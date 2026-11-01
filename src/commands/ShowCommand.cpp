#include "commands/Commands.h"
#include "core/Repository.h"
#include "services/DiffService.h"
#include "utils/Color.h"
#include "utils/Hash.h"
#include "utils/Exceptions.h"
#include <iostream>

namespace minigit {

int cmd_show(const ParsedArgs& args) {
    try {
        auto repo = Repository::open();
        std::string ref = args.positional.empty() ? "HEAD" : args.positional[0];

        std::string commitHash;
        if (ref == "HEAD") {
            commitHash = repo.refs().resolveHead();
            if (commitHash.empty()) {
                std::cout << Color::yellow("No commits yet.\n");
                return 0;
            }
        } else if (Hash::isValidHash(ref)) {
            commitHash = ref;
        } else {
            // try branch
            commitHash = repo.refs().readBranch(ref);
            if (commitHash.empty()) {
                commitHash = repo.objectStore().resolveAbbrev(ref);
            }
            if (commitHash.empty()) {
                std::cerr << Color::boldRed("error: ") << "unknown ref '" << ref << "'\n";
                return 1;
            }
        }

        auto commit = repo.objectStore().loadCommit(commitHash);
        std::cout << Color::boldYellow("commit ") << Color::yellow(commitHash) << "\n";
        if (commit.parentHash().has_value())
            std::cout << Color::gray("parent " + *commit.parentHash()) << "\n";
        std::cout << Color::bold("Author: ") << commit.author() << " <" << commit.email() << ">\n";
        std::cout << Color::bold("Date:   ") << commit.timestamp() << "\n";
        std::cout << "\n    " << commit.message() << "\n\n";

        // Show diff vs parent
        auto tree = repo.flattenTree(commit.treeHash());
        std::unordered_map<std::string,std::string> parentFiles;
        if (commit.parentHash().has_value()) {
            auto parentCommit = repo.objectStore().loadCommit(*commit.parentHash());
            parentFiles = repo.flattenTree(parentCommit.treeHash());
        }

        for (auto& [path, hash] : tree) {
            std::string oldContent;
            auto it = parentFiles.find(path);
            if (it != parentFiles.end())
                oldContent = repo.objectStore().loadBlob(it->second).content();
            auto newContent = repo.objectStore().loadBlob(hash).content();
            if (oldContent == newContent) continue;
            auto d = diffTexts(path, path, oldContent, newContent);
            std::cout << formatDiff(d, Color::enabled());
        }
        return 0;
    } catch (const MiniGitException& e) {
        std::cerr << Color::boldRed("fatal: ") << e.what() << "\n";
        return 1;
    }
}

} // namespace minigit
