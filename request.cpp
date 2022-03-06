#include "request.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <optional>

#include "helper.h"

namespace Request {

std::optional<size_t> body_details(const std::string& received) {
    std::string::size_type n = received.find("Content-Length: ");

    if (n == std::string::npos) {
        return {};
    }

    std::string length;
    auto it = received.begin() + n + CONTENT_LENGTH_SIZE;

    while (*it != '\r') {
        length += *it;
        advance(it, 1);
    }

    return std::stoi(length);
}

std::string send_buffer(const char* host, uint16_t port,
                        const char* send_buffer, size_t send_size) {
    int result, request_socket;
    ssize_t transfered;

    request_socket = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT(request_socket != -1, "failed to open socket");

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host);

    result = connect(request_socket, (struct sockaddr*)&addr, sizeof(addr));
    ASSERT(result != -1, "failed connect");

    size_t accum = 0;
    do {
        transfered =
            send(request_socket, send_buffer + accum, send_size - accum, 0);
        ASSERT(transfered != -1, "failed to send request");

        if (transfered == 0) {
            break;
        }

        accum += transfered;
    } while (accum < send_size);

    std::string receive;

    std::vector<char> buffer;
    buffer.resize(65536);

    do {
        transfered = recv(request_socket, buffer.data(), buffer.size(), 0);
        ASSERT(transfered != -1, "failed to receive request response");

        receive += std::string(buffer.data(), transfered);

        std::string::size_type n;
        if ((n = receive.find("\r\n\r\n")) != std::string::npos) {
            auto details = body_details(receive);

            if (!details) {
                break;
            }

            if (!details.value()) {
                break;
            }

            if (details.value() == receive.size() - n - 4) {
                break;
            }
        }

        if (transfered == 0) {
            break;
        }
    } while (true);

    return receive;
}

void format_request(std::stringstream& stream, const char* verb,
                    const char* path, const Headers& headers,
                    const std::string& body) {
    stream << verb << " " << path << " HTTP/1.1\r\n";

    for (const auto& tuple : headers) {
        stream << tuple.first << ": " << tuple.second << "\r\n";
    }

    if (std::string(verb) != "GET") {
        stream << "Content-Length: " << std::to_string(body.size()) << "\r\n";
    }

    stream << "Host: 34.118.48.238\r\n";
    stream << "\r\n";

    if (body.size()) {
        stream << body;
    }
}

void parse_response(Response& response, std::stringstream& stream) {
    std::vector<char> buffer;
    buffer.resize(32);

    stream.ignore(MAX_PROTOCOL_LENGTH, ' ');
    stream >> response.status_code;
    stream.ignore(1);
    stream.getline(buffer.data(), buffer.size(), '\r');

    response.status = std::string(buffer.data(), buffer.size());

    buffer.resize(MAX_HEADER_SIZE);

    while (stream.peek() != '\r') {
        stream.getline(buffer.data(), MAX_HEADER_SIZE, ':');
        std::string key(buffer.data(), stream.gcount() - 1);

        stream.get();

        stream.getline(buffer.data(), MAX_HEADER_SIZE, '\r');
        std::string value(buffer.data(), stream.gcount() - 1);

        stream.get();
        response.headers[key] = value;
    }

    stream.get();
    stream.get();

    buffer.resize(MAX_REQUEST_BODY);

    stream.read(buffer.data(), MAX_REQUEST_BODY);
    uint32_t length = stream.gcount();

    buffer.resize(length);
    response.body = std::string(buffer.data(), buffer.size());
}

Response request(const char* verb, const char* path, const Headers& headers,
                 const std::string& body) {
    std::stringstream request_stream;
    format_request(request_stream, verb, path, headers, body);

    std::string request = request_stream.str();

    // std::cout << "------- REQUEST -------\n"
    //           << std::string(request.data(), request.size()) << std::endl;

    std::string received =
        send_buffer("34.118.48.238", 8080, request.data(), request.size());

    // std::cout << "------- RESPONSE -------\n" << buffer.data() << std::endl;

    Response response;
    std::stringstream response_stream(received);

    parse_response(response, response_stream);
    return response;
}

}  // namespace Request