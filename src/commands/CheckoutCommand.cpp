#include "commands/Commands.h"
#include "core/Repository.h"
#include "utils/Color.h"
#include "utils/FileSystem.h"
#include "utils/Exceptions.h"
#include <iostream>

namespace minigit {

int cmd_checkout(const ParsedArgs& args) {
    try {
        auto repo = Repository::open();
        if (args.positional.empty()) {
            std::cerr << Color::boldRed("error: ") << "specify a branch or commit to checkout\n";
            return 1;
        }

        bool createNew = args.options.contains("b");
        auto target = args.positional[0];

        if (createNew) {
            // Create and switch to new branch
            if (repo.refs().branchExists(target)) {
                std::cerr << Color::boldRed("error: ") << "branch '" << target << "' already exists\n";
                return 1;
            }
            auto headHash = repo.refs().resolveHead();
            if (!headHash.empty())
                repo.refs().writeBranch(target, headHash);
            repo.refs().setHead(target);
            std::cout << Color::boldGreen("Switched to a new branch '") << Color::boldCyan(target) << Color::boldGreen("'\n");
            return 0;
        }

        // Switch to existing branch
        if (repo.refs().branchExists(target)) {
            auto branchHash = repo.refs().readBranch(target);
            // Update working tree and index to match the branch HEAD
            if (!branchHash.empty()) {
                auto commit   = repo.objectStore().loadCommit(branchHash);
                auto headFiles = repo.flattenTree(commit.treeHash());
                // Restore all files
                for (auto& [path, hash] : headFiles) {
                    auto content = repo.objectStore().loadBlob(hash).content();
                    FileSystem::writeFileAtomic(repo.toAbsolute(path), content);
                }
                // Update index
                repo.index().clear();
                for (auto& [path, hash] : headFiles) {
                    IndexEntry ie; ie.path = path; ie.hash = hash; ie.mode = 100644;
                    repo.index().addEntry(ie);
                }
                repo.index().save();
            }
            repo.refs().setHead(target);
            std::cout << Color::boldGreen("Switched to branch '") << Color::boldCyan(target) << Color::boldGreen("'\n");
            return 0;
        }

        std::cerr << Color::boldRed("error: ") << "branch '" << target << "' not found\n";
        return 1;
    } catch (const MiniGitException& e) {
        std::cerr << Color::boldRed("fatal: ") << e.what() << "\n";
        return 1;
    }
}

} // namespace minigit
