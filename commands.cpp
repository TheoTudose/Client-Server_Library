#include "commands.h"

#include <iostream>
#include <vector>

#include "actions.h"

namespace Commands {

std::optional<std::string> session;
std::optional<std::string> jwt;

void query(std::map<std::string, std::string>& inputs,
           std::vector<std::string> queries) {
    for (const auto& query : queries) {
        std::cout << query << "=";

        std::string input;
        do {
            std::getline(std::cin, input);
        } while (!input.size());

        inputs[query] = input;
    }
}

std::map<std::string, std::string> match(const std::string& name) {
    std::map<std::string, std::string> out;

    if (name == "register") {
        query(out, {"username", "password"});
    } else if (name == "login") {
        query(out, {"username", "password"});
    } else if (name == "enter_library") {
        query(out, {});
    } else if (name == "get_books") {
        query(out, {});
    } else if (name == "get_book") {
        query(out, {"id"});
    } else if (name == "add_book") {
        query(out, {"title", "author", "genre", "publisher", "page_count"});
    } else if (name == "delete_book") {
        query(out, {"id"});
    } else if (name == "logout") {
        query(out, {});
    }

    return out;
}

Result exec(const std::string& name,
            std::map<std::string, std::string>& inputs) {
    Actions::Result result = {false};

    if (name == "register") {
        result = Actions::create(inputs["username"], inputs["password"]);
        return result.is_ok() ? Result() : Result(result.data());
    } else if (name == "login") {
        result = Actions::authenticate(inputs["username"], inputs["password"]);

        if (result.is_ok()) {
            session = result.data();
        }

        return result.is_ok() ? Result() : Result(result.data());
    } else if (name == "enter_library") {
        result = Actions::access(session);

        if (result.is_ok()) {
            jwt = result.data();
        }

        return result.is_ok() ? Result() : Result(result.data());
    } else if (name == "get_books") {
        result = Actions::books(jwt);
        return Result(result.data());
    } else if (name == "get_book") {
        result = Actions::book(jwt, inputs["id"]);
        return Result(result.data());
    } else if (name == "add_book") {
        result = Actions::add_book(jwt, inputs["title"], inputs["author"],
                                   inputs["genre"], inputs["page_count"],
                                   inputs["publisher"]);
        return result.is_ok() ? Result() : Result(result.data());
    } else if (name == "delete_book") {
        result = Actions::delete_book(jwt, inputs["id"]);
        return result.is_ok() ? Result() : Result(result.data());
    } else if (name == "logout") {
        result = Actions::logout(session);
        return result.is_ok() ? Result() : Result(result.data());
    }

    return Result();
}

}  // namespace Commands