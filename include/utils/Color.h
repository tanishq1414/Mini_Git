#pragma once
#include <string>
#include <string_view>

namespace minigit {

/// Central color/formatting utility.
/// Uses ANSI escape codes with Windows VT mode detection.
/// Respects the --no-color flag via Color::disable().
class Color {
public:
    // ANSI escape codes
    static constexpr std::string_view RESET   = "\033[0m";
    static constexpr std::string_view BOLD    = "\033[1m";
    static constexpr std::string_view DIM     = "\033[2m";

    static constexpr std::string_view RED     = "\033[31m";
    static constexpr std::string_view GREEN   = "\033[32m";
    static constexpr std::string_view YELLOW  = "\033[33m";
    static constexpr std::string_view BLUE    = "\033[34m";
    static constexpr std::string_view MAGENTA = "\033[35m";
    static constexpr std::string_view CYAN    = "\033[36m";
    static constexpr std::string_view WHITE   = "\033[37m";
    static constexpr std::string_view GRAY    = "\033[90m";

    static constexpr std::string_view BOLD_RED    = "\033[1;31m";
    static constexpr std::string_view BOLD_GREEN  = "\033[1;32m";
    static constexpr std::string_view BOLD_YELLOW = "\033[1;33m";
    static constexpr std::string_view BOLD_CYAN   = "\033[1;36m";
    static constexpr std::string_view BOLD_WHITE  = "\033[1;37m";

    /// Initialize color support (enables VT processing on Windows).
    static void init();

    /// Disable all color output (--no-color flag).
    static void disable() noexcept;

    /// Returns true if color output is enabled.
    static bool enabled() noexcept;

    /// Wrap text in a color code (returns plain text if disabled).
    static std::string apply(std::string_view code, std::string_view text);

    // Convenience helpers
    static std::string red    (std::string_view t) { return apply(RED,     t); }
    static std::string green  (std::string_view t) { return apply(GREEN,   t); }
    static std::string yellow (std::string_view t) { return apply(YELLOW,  t); }
    static std::string cyan   (std::string_view t) { return apply(CYAN,    t); }
    static std::string gray   (std::string_view t) { return apply(GRAY,    t); }
    static std::string bold   (std::string_view t) { return apply(BOLD,    t); }
    static std::string boldGreen(std::string_view t) { return apply(BOLD_GREEN,  t); }
    static std::string boldRed  (std::string_view t) { return apply(BOLD_RED,    t); }
    static std::string boldCyan (std::string_view t) { return apply(BOLD_CYAN,   t); }
    static std::string boldYellow(std::string_view t){ return apply(BOLD_YELLOW, t); }
    static std::string boldWhite(std::string_view t) { return apply(BOLD_WHITE,  t); }

private:
    static bool s_enabled;
};

} // namespace minigit
