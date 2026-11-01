#include "commands/Commands.h"
#include "core/Repository.h"
#include "utils/Color.h"
#include "utils/Hash.h"
#include "utils/Exceptions.h"
#include "utils/FileSystem.h"
#include <iostream>
#include <filesystem>

namespace minigit {

int cmd_add(const ParsedArgs& args) {
    try {
        auto repo = Repository::open();

        if (args.positional.empty() && !args.options.contains("all") && !args.options.contains("A")) {
            std::cerr << Color::boldRed("error: ") << "Nothing specified. Use 'minigit add <file>' or 'minigit add --all'\n";
            return 1;
        }

        auto addFile = [&](const std::string& relPath) {
            auto absPath = repo.toAbsolute(relPath);
            if (!std::filesystem::exists(absPath)) {
                std::cerr << Color::boldRed("error: ") << "pathspec '" << relPath << "' did not match any files\n";
                return;
            }
            auto content = FileSystem::readFile(absPath);
            Blob blob(content);
            auto hash = repo.objectStore().store(blob);
            IndexEntry ie;
            ie.path   = relPath;
            ie.hash   = hash;
            ie.mode   = 100644;
            ie.action = StagedAction::Add;
            repo.index().addEntry(ie);
            std::cout << Color::boldGreen("add ") << relPath << "\n";
        };

        if (args.options.contains("all") || args.options.contains("A")) {
            // Add all tracked + untracked files
            auto files = FileSystem::listFilesRecursive(repo.root(), [&](const std::filesystem::path& p){
                return !repo.isIgnored(p);
            });
            for (auto& f : files) {
                try {
                    auto rel = repo.toRepoRelative(f);
                    addFile(rel);
                } catch (...) {}
            }
        } else {
            for (auto& arg : args.positional) {
                // Handle globbing: if arg ends with * or is a directory
                std::string rel;
                try {
                    rel = repo.toRepoRelative(std::filesystem::path(arg));
                } catch (...) {
                    std::cerr << Color::boldRed("error: ") << "'" << arg << "' is outside the repository\n";
                    continue;
                }
                auto absPath = repo.toAbsolute(rel);
                if (std::filesystem::is_directory(absPath)) {
                    auto files = FileSystem::listFilesRecursive(absPath, [&](const std::filesystem::path& p){
                        return !repo.isIgnored(p);
                    });
                    for (auto& f : files) {
                        try { addFile(repo.toRepoRelative(f)); } catch (...) {}
                    }
                } else {
                    addFile(rel);
                }
            }
        }
        repo.index().save();
        return 0;
    } catch (const MiniGitException& e) {
        std::cerr << Color::boldRed("fatal: ") << e.what() << "\n";
        return 1;
    }
}

} // namespace minigit
