#ifndef COMMANDS_H
#define COMMANDS_H

#include <map>
#include <optional>
#include <string>

namespace Commands {

typedef std::optional<std::string> Result;

std::map<std::string, std::string> match(const std::string& name);
Result exec(const std::string& name,
            std::map<std::string, std::string>& inputs);

}  // namespace Commands

#endif