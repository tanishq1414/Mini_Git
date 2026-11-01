#include "utils/Config.h"
#include "utils/FileSystem.h"
#include <nlohmann/json.hpp>
#include <fstream>

namespace minigit {

using json = nlohmann::json;

Config::Config(const std::filesystem::path& repoRoot)
    : m_configPath(repoRoot / ".minigit" / "config")
    , m_authorName("Unknown")
    , m_authorEmail("unknown@example.com")
    , m_defaultBranch("main")
{
    if (std::filesystem::exists(m_configPath)) {
        try { load(); } catch (...) {}
    }
}

void Config::load() {
    auto raw = FileSystem::readFile(m_configPath);
    auto j = json::parse(raw);
    m_authorName    = j.value("author_name",    "Unknown");
    m_authorEmail   = j.value("author_email",   "unknown@example.com");
    m_defaultBranch = j.value("default_branch", "main");
}

void Config::save() const {
    json j = {
        {"author_name",    m_authorName},
        {"author_email",   m_authorEmail},
        {"default_branch", m_defaultBranch}
    };
    FileSystem::writeFileAtomic(m_configPath, j.dump(2));
}

std::string Config::getAuthorName()    const { return m_authorName; }
std::string Config::getAuthorEmail()   const { return m_authorEmail; }
std::string Config::getDefaultBranch() const { return m_defaultBranch; }

void Config::setAuthorName(const std::string& name) {
    m_authorName = name;
    save();
}
void Config::setAuthorEmail(const std::string& email) {
    m_authorEmail = email;
    save();
}

} // namespace minigit
