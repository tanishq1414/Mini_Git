#include "commands/Commands.h"
#include "core/Repository.h"
#include "utils/Color.h"
#include "utils/Hash.h"
#include "utils/FileSystem.h"
#include "utils/Exceptions.h"
#include <iostream>

namespace minigit {

int cmd_reset(const ParsedArgs& args) {
    try {
        auto repo = Repository::open();

        // If positional args, unstage specific files (mixed reset for files)
        if (!args.positional.empty()) {
            auto headHash = repo.refs().resolveHead();
            std::unordered_map<std::string,std::string> headFiles;
            if (!headHash.empty()) {
                auto commit = repo.objectStore().loadCommit(headHash);
                headFiles = repo.flattenTree(commit.treeHash());
            }
            for (auto& arg : args.positional) {
                std::string rel;
                try { rel = repo.toRepoRelative(arg); } catch (...) {
                    std::cerr << Color::boldRed("error: ") << "'" << arg << "' outside repo\n";
                    continue;
                }
                auto it = headFiles.find(rel);
                if (it == headFiles.end()) {
                    repo.index().removeEntry(rel);
                } else {
                    IndexEntry ie; ie.path = rel; ie.hash = it->second; ie.mode = 100644;
                    repo.index().addEntry(ie);
                }
                std::cout << Color::boldYellow("unstaged ") << rel << "\n";
            }
            repo.index().save();
            return 0;
        }

        // Determine mode
        std::string mode = "mixed";
        if (args.options.contains("soft"))  mode = "soft";
        if (args.options.contains("hard"))  mode = "hard";
        if (args.options.contains("mixed")) mode = "mixed";

        auto headHash = repo.refs().resolveHead();
        if (headHash.empty()) {
            std::cerr << Color::boldRed("error: ") << "no commits to reset to\n";
            return 1;
        }
        auto commit = repo.objectStore().loadCommit(headHash);
        auto headFiles = repo.flattenTree(commit.treeHash());

        if (mode == "mixed" || mode == "hard") {
            // Reset index to HEAD
            repo.index().clear();
            for (auto& [path, hash] : headFiles) {
                IndexEntry ie; ie.path = path; ie.hash = hash; ie.mode = 100644;
                repo.index().addEntry(ie);
            }
            repo.index().save();
        }
        if (mode == "hard") {
            // Reset working tree to HEAD
            for (auto& [path, hash] : headFiles) {
                auto content = repo.objectStore().loadBlob(hash).content();
                FileSystem::writeFileAtomic(repo.toAbsolute(path), content);
            }
        }
        std::cout << Color::boldGreen("HEAD is now at ") << Hash::abbreviate(headHash,8) << " (--" << mode << ")\n";
        return 0;
    } catch (const MiniGitException& e) {
        std::cerr << Color::boldRed("fatal: ") << e.what() << "\n";
        return 1;
    }
}

} // namespace minigit
