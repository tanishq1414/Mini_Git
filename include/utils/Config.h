#pragma once
#include <string>
#include <filesystem>

namespace minigit {

/// Repository configuration (author, email, core settings).
class Config {
public:
    explicit Config(const std::filesystem::path& repoRoot);

    std::string getAuthorName() const;
    std::string getAuthorEmail() const;
    std::string getDefaultBranch() const;

    void setAuthorName(const std::string& name);
    void setAuthorEmail(const std::string& email);

    void load();
    void save() const;

private:
    std::filesystem::path m_configPath;
    std::string m_authorName;
    std::string m_authorEmail;
    std::string m_defaultBranch;
};

} // namespace minigit
