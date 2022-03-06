#include <iostream>
#include <string>

#include "actions.h"
#include "request.h"
#include "commands.h"
#include "json.h"

using json = nlohmann::json;

int main() {
    std::string name;

    for (;;) {
        std::cin >> name;

        if (name == "exit") {
            return 0;
        }

        auto inputs = Commands::match(name);
        auto output = Commands::exec(name, inputs);

        if (output) {
            std::cout << output.value() << std::endl;
        }
    }

    return 0;
}