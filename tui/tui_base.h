#pragma once


#include <vector>
#include <deque>
#include <string>
#include <string_view>
#include <stack>
#include <sstream>

#include "../result.h"


namespace dungeons::tui {


    enum class StyleOperation : uint16_t {
        NONE = 0,
        SET_FG_COLOR = 1 << 0,
        SET_BG_COLOR = 1 << 1,
        SET_STYLE = 1 << 2,
        RESET_STYLE = 1 << 3
    };

    // Побитовые операции для StyleOperation
    constexpr StyleOperation operator|(StyleOperation a, StyleOperation b) noexcept {
        return static_cast<StyleOperation>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
    }
    constexpr StyleOperation operator&(StyleOperation a, StyleOperation b) noexcept {
        return static_cast<StyleOperation>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b));
    }
    constexpr bool has_operation(StyleOperation flags, StyleOperation flag) noexcept {
        return (flags & flag) == flag && flag != StyleOperation::NONE;
    }


    enum class Color {
        BLACK,
        RED,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN,
        WHITE,

        GREY,
        BRIGHT_RED,
        BRIGHT_GREEN,
        BRIGHT_YELLOW,
        BRIGHT_BLUE,
        BRIGHT_MAGENTA,
        BRIGHT_CYAN,
        BRIGHT_WHITE,

        RGB,
        ANSI8
    };


    enum class TextStyle : int {
        BOLD = 1 << 0,
        DIM = 1 << 1,
        ITALIC = 1 << 2,
        UNDERLINED = 1 << 3,
        BLINK_SLOW = 1 << 4,
        BLINK_FAST = 1 << 5,
        INVERSE = 1 << 6,
        HIDDEN = 1 << 7,
        STRIKE_THROUGH = 1 << 8,
        NONE = 0
    };

    // Побитовые операции для TextStyle
    constexpr TextStyle operator|(TextStyle a, TextStyle b) noexcept {
        return static_cast<TextStyle>(static_cast<int>(a) | static_cast<int>(b));
    }
    constexpr TextStyle operator&(TextStyle a, TextStyle b) noexcept {
        return static_cast<TextStyle>(static_cast<int>(a) & static_cast<int>(b));
    }
    constexpr bool has_style(TextStyle styles, TextStyle style) noexcept {
        return (styles & style) == style && style != TextStyle::NONE;
    }


}