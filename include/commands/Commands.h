#pragma once
#include "cli/CLIParser.h"

namespace minigit {
int cmd_init(const ParsedArgs& args);
int cmd_status(const ParsedArgs& args);
int cmd_add(const ParsedArgs& args);
int cmd_commit(const ParsedArgs& args);
int cmd_log(const ParsedArgs& args);
int cmd_diff(const ParsedArgs& args);
int cmd_restore(const ParsedArgs& args);
int cmd_reset(const ParsedArgs& args);
int cmd_branch(const ParsedArgs& args);
int cmd_checkout(const ParsedArgs& args);
int cmd_show(const ParsedArgs& args);
int cmd_rm(const ParsedArgs& args);
int cmd_graph(const ParsedArgs& args);
} // namespace minigit
