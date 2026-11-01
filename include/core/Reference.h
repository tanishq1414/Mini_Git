#pragma once
#include <filesystem>
#include <string>
#include <optional>
#include <vector>

namespace minigit {

/// Manages HEAD file and branch refs in .minigit/refs/heads/.
class Reference {
public:
    explicit Reference(const std::filesystem::path& minigitDir);

    /// Read HEAD. Returns branch name if symbolic (refs/heads/X) or commit hash if detached.
    std::string readHead() const;

    /// Returns true if HEAD points to a branch (symbolic ref).
    bool isSymbolicHead() const;

    /// Returns branch name if symbolic, empty string otherwise.
    std::string currentBranch() const;

    /// Returns the commit hash HEAD ultimately resolves to, or empty string if no commits yet.
    std::string resolveHead() const;

    /// Read commit hash for a branch name. Returns empty if branch has no commits.
    std::string readBranch(const std::string& branch) const;

    /// Write commit hash to branch ref.
    void writeBranch(const std::string& branch, const std::string& commitHash);

    /// Update HEAD to point to a branch (symbolic ref).
    void setHead(const std::string& branch);

    /// Set HEAD to a detached commit.
    void setDetachedHead(const std::string& commitHash);

    /// List all local branches.
    std::vector<std::string> listBranches() const;

    /// Check if a branch exists.
    bool branchExists(const std::string& branch) const;

    /// Delete a branch ref file.
    void deleteBranch(const std::string& branch);

    /// Advance current branch to point to newCommitHash.
    void advanceBranch(const std::string& commitHash);

private:
    std::filesystem::path m_minigitDir;
    std::filesystem::path headsDir() const;
    std::filesystem::path branchPath(const std::string& branch) const;
};

} // namespace minigit
