#include "core/Commit.h"
#include <nlohmann/json.hpp>

namespace minigit {

using json = nlohmann::json;

Commit::Commit(std::string treeHash,
               std::optional<std::string> parentHash,
               std::string author,
               std::string email,
               std::string timestamp,
               std::string message)
    : m_treeHash(std::move(treeHash))
    , m_parentHash(std::move(parentHash))
    , m_author(std::move(author))
    , m_email(std::move(email))
    , m_timestamp(std::move(timestamp))
    , m_message(std::move(message))
{}

std::string Commit::serialize() const {
    json j = {
        {"tree",      m_treeHash},
        {"author",    m_author},
        {"email",     m_email},
        {"timestamp", m_timestamp},
        {"message",   m_message}
    };
    if (m_parentHash.has_value())
        j["parent"] = *m_parentHash;
    else
        j["parent"] = nullptr;
    return "commit\n" + j.dump(2);
}

Commit Commit::deserialize(const std::string& data) {
    auto pos = data.find('\n');
    if (pos == std::string::npos)
        throw std::runtime_error("invalid commit data");
    auto j = json::parse(data.substr(pos + 1));

    std::optional<std::string> parent;
    if (!j["parent"].is_null())
        parent = j["parent"].get<std::string>();

    return Commit(
        j["tree"].get<std::string>(),
        parent,
        j["author"].get<std::string>(),
        j["email"].get<std::string>(),
        j["timestamp"].get<std::string>(),
        j["message"].get<std::string>()
    );
}

} // namespace minigit
