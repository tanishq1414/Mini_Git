#include "core/Index.h"
#include "utils/FileSystem.h"
#include "utils/Exceptions.h"
#include <nlohmann/json.hpp>

namespace minigit {

using json = nlohmann::json;

Index::Index(const std::filesystem::path& indexPath)
    : m_path(indexPath) {}

void Index::load() {
    m_entries.clear();
    if (!std::filesystem::exists(m_path)) return;
    try {
        auto raw = FileSystem::readFile(m_path);
        if (raw.empty()) return;
        auto j = json::parse(raw);
        for (auto& item : j) {
            IndexEntry e;
            e.path   = item["path"].get<std::string>();
            e.hash   = item["hash"].get<std::string>();
            e.mode   = item.value("mode", 100644);
            auto act = item.value("action", "add");
            e.action = (act == "delete") ? StagedAction::Delete : StagedAction::Add;
            m_entries[e.path] = e;
        }
    } catch (...) {
        throw IndexException("corrupt index file");
    }
}

void Index::save() const {
    json j = json::array();
    for (auto& [path, e] : m_entries) {
        j.push_back({
            {"path",   e.path},
            {"hash",   e.hash},
            {"mode",   e.mode},
            {"action", e.action == StagedAction::Delete ? "delete" : "add"}
        });
    }
    FileSystem::ensureDir(m_path.parent_path());
    FileSystem::writeFileAtomic(m_path, j.dump(2));
}

void Index::addEntry(IndexEntry entry) {
    m_entries[entry.path] = std::move(entry);
}

bool Index::removeEntry(const std::string& path) {
    return m_entries.erase(path) > 0;
}

std::optional<IndexEntry> Index::getEntry(const std::string& path) const {
    auto it = m_entries.find(path);
    if (it == m_entries.end()) return std::nullopt;
    return it->second;
}

bool Index::hasEntry(const std::string& path) const {
    return m_entries.contains(path);
}

void Index::clear() {
    m_entries.clear();
}

void Index::commit(const std::unordered_map<std::string, IndexEntry>& committedEntries) {
    m_entries = committedEntries;
}

} // namespace minigit
