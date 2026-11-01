#pragma once
#include "core/Repository.h"
#include <string>

namespace minigit {

/// Renders an ASCII commit history graph.
std::string renderGraph(Repository& repo, int maxCommits = 30);

} // namespace minigit
