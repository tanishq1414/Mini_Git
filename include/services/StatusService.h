#pragma once
#include "core/Repository.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace minigit {

enum class FileStatus {
    Untracked,      ///< Not in index, not in HEAD
    Modified,       ///< In index, different from working tree
    Staged,         ///< In index (new or different from HEAD)
    Deleted,        ///< In HEAD/index but gone from working tree
    Unmodified      ///< Same in working tree, index, and HEAD
};

struct FileStatusEntry {
    std::string path;
    FileStatus  workingTree;    ///< Working tree vs index
    FileStatus  stagingArea;    ///< Index vs HEAD
};

struct RepoStatus {
    std::string currentBranch;
    bool        detached = false;
    bool        hasCommits = false;
    std::vector<FileStatusEntry> entries;
};

class StatusService {
public:
    static RepoStatus compute(Repository& repo);
private:
    static std::unordered_map<std::string,std::string> workingTreeFiles(Repository& repo);
};

} // namespace minigit
