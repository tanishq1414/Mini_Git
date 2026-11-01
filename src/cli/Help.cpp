#include "cli/Help.h"
#include "utils/Color.h"
#include <iostream>

namespace minigit {

void printGeneralHelp() {
    std::cout << Color::boldCyan("MiniGit") << Color::bold(" - A lightweight version control system\n\n");
    std::cout << Color::bold("Usage: ") << "minigit <command> [options]\n\n";
    std::cout << Color::bold("Commands:\n");
    auto row = [](const std::string& cmd, const std::string& desc) {
        std::cout << "  " << Color::boldGreen(cmd)
                  << std::string(16 - cmd.size(), ' ')
                  << desc << "\n";
    };
    row("init",     "Initialize a new repository");
    row("status",   "Show working tree status");
    row("add",      "Stage files (use --all to stage everything)");
    row("commit",   "Record changes (-m <message>)");
    row("log",      "Show commit history");
    row("diff",     "Show unstaged or staged (--staged) changes");
    row("restore",  "Restore working tree or index file");
    row("reset",    "Reset index/working tree (--soft|--mixed|--hard)");
    row("branch",   "List, create (-b), or delete (-d) branches");
    row("checkout", "Switch branch or create+switch (-b)");
    row("show",     "Show commit details and diff");
    row("rm",       "Remove file from index (--cached) or disk");
    row("graph",    "Show ASCII commit graph");
    row("help",     "Show this help or per-command help");
    std::cout << "\n";
}

void printCommandHelp(const std::string& command) {
    std::cout << Color::boldCyan("minigit " + command) << "\n\n";
    if (command == "add") {
        std::cout << "  minigit add <file>...    Stage specific files\n";
        std::cout << "  minigit add --all        Stage all changes\n";
    } else if (command == "commit") {
        std::cout << "  minigit commit -m <msg>  Commit staged changes\n";
    } else if (command == "reset") {
        std::cout << "  minigit reset <file>     Unstage a file\n";
        std::cout << "  minigit reset --soft     Keep index and worktree\n";
        std::cout << "  minigit reset --mixed    Reset index only (default)\n";
        std::cout << "  minigit reset --hard     Reset index and working tree\n";
    } else if (command == "diff") {
        std::cout << "  minigit diff             Working tree vs index\n";
        std::cout << "  minigit diff --staged    Index vs HEAD\n";
    } else if (command == "restore") {
        std::cout << "  minigit restore <file>            Restore from index\n";
        std::cout << "  minigit restore --staged <file>   Unstage from HEAD\n";
    } else if (command == "branch") {
        std::cout << "  minigit branch           List branches\n";
        std::cout << "  minigit branch <name>    Create branch\n";
        std::cout << "  minigit branch -d <name> Delete branch\n";
    } else if (command == "checkout") {
        std::cout << "  minigit checkout <branch>    Switch branch\n";
        std::cout << "  minigit checkout -b <name>   Create + switch\n";
    } else if (command == "rm") {
        std::cout << "  minigit rm <file>          Remove from index + disk\n";
        std::cout << "  minigit rm --cached <file> Remove from index only\n";
    } else {
        printGeneralHelp();
    }
}

} // namespace minigit
