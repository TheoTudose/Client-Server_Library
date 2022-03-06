#ifndef ACTIONS_H
#define ACTIONS_H

#include <optional>
#include <string>

namespace Actions {

class Result {
    bool m_ok;
    std::string m_data;

   public:
    Result(bool ok);
    Result(bool ok, std::string data);
    std::string& data();
    bool is_ok();
};

Result create(const std::string& username, const std::string& password);
Result authenticate(const std::string& username, const std::string& password);
Result access(const std::optional<std::string>& session);
Result books(const std::optional<std::string>& jwt);
Result book(const std::optional<std::string>& jwt, const std::string& id);
Result add_book(const std::optional<std::string>& jwt, const std::string& title,
                const std::string& author, const std::string& genre,
                const std::string& page_count, const std::string& publisher);
Result delete_book(const std::optional<std::string>& jwt,
                   const std::string& id);
Result logout(const std::optional<std::string>& session);

}  // namespace Actions

#endif