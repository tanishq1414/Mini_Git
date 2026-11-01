#pragma once
#include <string>
#include <vector>

namespace minigit {

struct DiffLine {
    enum class Kind { Context, Added, Removed };
    Kind        kind;
    std::string text;
    int         oldLine;
    int         newLine;
};

struct DiffHunk {
    int                  oldStart, oldCount;
    int                  newStart, newCount;
    std::vector<DiffLine> lines;
};

struct FileDiff {
    std::string            oldPath;
    std::string            newPath;
    std::vector<DiffHunk>  hunks;
    bool                   isBinary = false;
    bool                   isNew    = false;
    bool                   isDeleted = false;
};

/// Compute line-by-line LCS diff between two text contents.
FileDiff diffTexts(const std::string& oldPath, const std::string& newPath,
                   const std::string& oldContent, const std::string& newContent);

/// Format a FileDiff as colored unified diff string.
std::string formatDiff(const FileDiff& diff, bool color = true);

} // namespace minigit
