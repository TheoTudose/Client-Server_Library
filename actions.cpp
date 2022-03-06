#include "actions.h"

#include <iostream>
#include <sstream>

#include "json.h"
#include "request.h"
#include "string.h"

using json = nlohmann::json;

namespace Actions {

Result::Result(bool ok) : m_ok(ok), m_data("") {}
Result::Result(bool ok, std::string data) : m_ok(ok), m_data(data) {}

bool Result::is_ok() { return m_ok; }

std::string& Result::data() { return m_data; }

Result create(const std::string& username, const std::string& password) {
    json body = {{"username", username}, {"password", password}};

    auto response =
        Request::request("POST", "/api/v1/tema/auth/register",
                         {{"Content-Type", "application/json"}}, body.dump());

    return {response.status_code == 200, response.body};
}

Result authenticate(const std::string& username, const std::string& password) {
    json body = {{"username", username}, {"password", password}};

    auto response =
        Request::request("POST", "/api/v1/tema/auth/login",
                         {{"Content-Type", "application/json"}}, body.dump());

    if (response.status_code == 200) {
        std::stringstream stream(response.headers["Set-Cookie"]);

        std::vector<char> buffer(MAX_HEADER_SIZE);
        stream.getline(buffer.data(), MAX_HEADER_SIZE, ';');
        buffer.resize(stream.gcount() - 1);

        return {true, std::string(buffer.data(), buffer.size())};
    } else {
        return {false, response.body};
    }
}

Result access(const std::optional<std::string>& session) {
    Request::Headers headers =
        session ? Request::Headers{{"Cookie", session.value()}}
                : Request::Headers{};

    auto response =
        Request::request("GET", "/api/v1/tema/library/access", headers, "");

    if (response.status_code == 200) {
        return {true, json::parse(response.body)["token"]};
    } else {
        return {false, response.body};
    }
}

Request::Headers authorization(const std::optional<std::string>& jwt) {
    return jwt ? Request::Headers{{"Authorization", "Bearer " + jwt.value()}}
               : Request::Headers{};
}

Result books(const std::optional<std::string>& jwt) {
    auto response = Request::request("GET", "/api/v1/tema/library/books",
                                     authorization(jwt), "");

    return {response.status_code == 200, response.body};
}

Result book(const std::optional<std::string>& jwt, const std::string& id) {
    std::string path = "/api/v1/tema/library/books/" + id;

    auto response =
        Request::request("GET", path.c_str(), authorization(jwt), "");

    return {response.status_code == 200, response.body};
}

Result add_book(const std::optional<std::string>& jwt, const std::string& title,
                const std::string& author, const std::string& genre,
                const std::string& page_count, const std::string& publisher) {
    json body = {{"title", title},
                 {"author", author},
                 {"genre", genre},
                 {"page_count", page_count},
                 {"publisher", publisher}};

    Request::Headers headers = authorization(jwt);
    headers["Content-Type"] = "application/json";

    auto response = Request::request("POST", "/api/v1/tema/library/books",
                                     headers, body.dump());

    return {response.status_code == 200, response.body};
}

Result delete_book(const std::optional<std::string>& jwt,
                   const std::string& id) {
    std::string path = "/api/v1/tema/library/books/" + id;

    auto response =
        Request::request("DELETE", path.c_str(), authorization(jwt), "");

    return {response.status_code == 200, response.body};
}

Result logout(const std::optional<std::string>& session) {
    Request::Headers headers =
        session ? Request::Headers{{"Cookie", session.value()}}
                : Request::Headers{};

    auto response =
        Request::request("GET", "/api/v1/tema/auth/logout", headers, "");

    return Result{response.status_code == 200, response.body};
}

}  // namespace Actions