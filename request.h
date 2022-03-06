#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <vector>
#include <map>

#define MAX_HEADER_SIZE 8192
#define MAX_REQUEST_BODY 131072
#define MAX_PROTOCOL_LENGTH 32

#define CONTENT_LENGTH_SIZE 16

namespace Request {

typedef std::map<std::string, std::string> Headers;

struct Response {
    uint32_t status_code;
    std::string status;
    Headers headers;
    std::string body;
};

Response request(const char* verb, const char* path,
                 const Headers& headers, const std::string& body);

}  // namespace Request

#endif