#include "core/Repository.h"
#include "utils/FileSystem.h"
#include "utils/Exceptions.h"
#include <fstream>
#include <algorithm>
#include <sstream>

namespace minigit {

Repository::Repository(std::filesystem::path root, std::filesystem::path minigitDir)
    : m_root(std::move(root))
    , m_minigitDir(std::move(minigitDir))
{
    m_objectStore = std::make_unique<ObjectStore>(m_minigitDir / "objects");
    m_index       = std::make_unique<Index>(m_minigitDir / "index");
    m_refs        = std::make_unique<Reference>(m_minigitDir);
    m_config      = std::make_unique<Config>(m_root);
    m_index->load();
    loadIgnorePatterns();
}

Repository Repository::open(const std::filesystem::path& startPath) {
    auto path = std::filesystem::absolute(startPath).lexically_normal();
    while (true) {
        auto candidate = path / ".minigit";
        if (std::filesystem::is_directory(candidate))
            return Repository(path, candidate);
        auto parent = path.parent_path();
        if (parent == path) break;
        path = parent;
    }
    throw NotARepositoryException();
}

Repository Repository::init(const std::filesystem::path& path) {
    auto root     = std::filesystem::absolute(path).lexically_normal();
    auto mgDir    = root / ".minigit";
    if (std::filesystem::exists(mgDir))
        throw CommandException("repository already exists at: " + root.string());

    std::filesystem::create_directories(mgDir / "objects");
    std::filesystem::create_directories(mgDir / "refs" / "heads");

    // Write initial HEAD
    FileSystem::writeFileAtomic(mgDir / "HEAD", "ref: refs/heads/main\n");

    return Repository(root, mgDir);
}

std::string Repository::toRepoRelative(const std::filesystem::path& p) const {
    auto abs = FileSystem::normalize(
        p.is_absolute() ? p : (std::filesystem::current_path() / p));
    if (!FileSystem::isInsideDirectory(abs, m_root))
        throw PathException("path is outside repository: " + p.string());
    auto rel = std::filesystem::relative(abs, m_root);
    // Convert to forward-slash string
    auto s = rel.generic_string();
    return s;
}

std::filesystem::path Repository::toAbsolute(const std::string& relPath) const {
    return m_root / relPath;
}

void Repository::loadIgnorePatterns() {
    m_ignorePatterns.clear();
    auto ignoreFile = m_root / ".minigitignore";
    if (!std::filesystem::exists(ignoreFile)) return;
    auto lines = FileSystem::splitLines(FileSystem::readFile(ignoreFile));
    for (auto& line : lines) {
        if (line.empty() || line[0] == '#') continue;
        m_ignorePatterns.push_back(line);
    }
}

bool Repository::isIgnored(const std::filesystem::path& absPath) const {
    // Always ignore .minigit directory contents
    if (FileSystem::isInsideDirectory(absPath, m_minigitDir)) return true;
    auto relStr = std::filesystem::relative(absPath, m_root).generic_string();
    for (auto& pat : m_ignorePatterns) {
        // Simple glob: startswith or endswith
        if (pat.front() == '*') {
            auto suffix = pat.substr(1);
            if (relStr.ends_with(suffix)) return true;
        } else if (relStr == pat || relStr.starts_with(pat + "/")) {
            return true;
        }
    }
    return false;
}

Tree Repository::buildTreeFromIndex() const {
    Tree t;
    for (auto& [path, entry] : m_index->entries()) {
        if (entry.action == StagedAction::Delete) continue;
        TreeEntry te;
        te.name = path;
        te.hash = entry.hash;
        te.type = ObjectType::Blob;
        te.mode = entry.mode;
        t.addEntry(te);
    }
    return t;
}

std::string Repository::buildAndStoreTree(const std::string& /*prefix*/) const {
    auto tree = buildTreeFromIndex();
    return m_objectStore->store(tree);
}

std::optional<Tree> Repository::headTree() const {
    auto commitHash = m_refs->resolveHead();
    if (commitHash.empty()) return std::nullopt;
    auto commit = m_objectStore->loadCommit(commitHash);
    return m_objectStore->loadTree(commit.treeHash());
}

std::unordered_map<std::string, std::string>
Repository::flattenTree(const std::string& treeHash, const std::string& prefix) const {
    std::unordered_map<std::string, std::string> result;
    auto tree = m_objectStore->loadTree(treeHash);
    for (auto& e : tree.entries()) {
        std::string fullPath = prefix.empty() ? e.name : prefix + "/" + e.name;
        if (e.type == ObjectType::Blob) {
            result[fullPath] = e.hash;
        } else if (e.type == ObjectType::Tree) {
            auto sub = flattenTree(e.hash, fullPath);
            result.insert(sub.begin(), sub.end());
        }
    }
    return result;
}

} // namespace minigit
