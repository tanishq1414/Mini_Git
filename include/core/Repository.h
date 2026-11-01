#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include "core/ObjectStore.h"
#include "core/Index.h"
#include "core/Reference.h"
#include "utils/Config.h"

namespace minigit {

/// The central repository object.
/// Locates the .minigit directory, verifies the repo, and provides
/// access to ObjectStore, Index, Reference, and Config.
class Repository {
public:
    /// Open an existing repository by searching upward from cwd.
    /// Throws NotARepositoryException if none found.
    static Repository open(const std::filesystem::path& startPath = std::filesystem::current_path());

    /// Initialize a new repository at the given path.
    /// Throws CommandException if already initialized.
    static Repository init(const std::filesystem::path& path);

    /// Returns root of working directory.
    const std::filesystem::path& root() const { return m_root; }

    /// Returns .minigit directory path.
    const std::filesystem::path& minigitDir() const { return m_minigitDir; }

    ObjectStore& objectStore() { return *m_objectStore; }
    const ObjectStore& objectStore() const { return *m_objectStore; }

    Index& index() { return *m_index; }
    const Index& index() const { return *m_index; }

    Reference& refs() { return *m_refs; }
    const Reference& refs() const { return *m_refs; }

    Config& config() { return *m_config; }
    const Config& config() const { return *m_config; }

    /// Convert a path (relative or absolute) to a repo-relative path string
    /// using forward slashes. Throws PathException if outside repo.
    std::string toRepoRelative(const std::filesystem::path& p) const;

    /// Convert a repo-relative path string to an absolute filesystem path.
    std::filesystem::path toAbsolute(const std::string& relPath) const;

    /// Check if a path matches .minigitignore patterns.
    bool isIgnored(const std::filesystem::path& absPath) const;

    /// Load ignore patterns from .minigitignore.
    void loadIgnorePatterns();

    /// Build a Tree object from current index entries (staged state).
    Tree buildTreeFromIndex() const;

    /// Recursively build Tree objects for a given directory prefix in the index.
    /// Returns hash of the root tree.
    std::string buildAndStoreTree(const std::string& prefix) const;

    /// Get the Tree for HEAD commit. Returns empty optional if no commits.
    std::optional<Tree> headTree() const;

    /// Walk a tree recursively and return all {relpath -> hash} pairs.
    std::unordered_map<std::string, std::string> flattenTree(
        const std::string& treeHash, const std::string& prefix = "") const;

private:
    Repository(std::filesystem::path root, std::filesystem::path minigitDir);

    std::filesystem::path m_root;
    std::filesystem::path m_minigitDir;
    std::unique_ptr<ObjectStore> m_objectStore;
    std::unique_ptr<Index> m_index;
    std::unique_ptr<Reference> m_refs;
    std::unique_ptr<Config> m_config;
    std::vector<std::string> m_ignorePatterns;
};

} // namespace minigit
