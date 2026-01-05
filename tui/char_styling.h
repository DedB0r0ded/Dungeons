#pragma once


#include "./tui_base.h"


namespace dungeons::tui {


    struct CharStyle {
        Color fg_color;
        int fg_ansi;
        int fg_r, fg_g, fg_b;

        Color bg_color;
        int bg_ansi;
        int bg_r, bg_g, bg_b;

        StyleFlags style;

        // Конструкторы
        constexpr CharStyle(Color fg_color, int fg_ansi, int fg_r, int fg_g, int fg_b, Color bg_color, int bg_ansi, int bg_r, int bg_g, int bg_b, StyleFlags style)
            noexcept : fg_color{ fg_color }, fg_ansi{ fg_ansi }, fg_r{ fg_r }, fg_g{ fg_g }, fg_b{ fg_b },
            bg_color{ bg_color }, bg_ansi{ bg_ansi }, bg_r{ bg_r }, bg_g{ bg_g }, bg_b { bg_b }, style{ style } { }

        CharStyle(const CharStyle& other) : 
            CharStyle(other.fg_color, other.fg_ansi, other.fg_r, other.fg_g, other.fg_b,
                other.bg_color, other.bg_ansi, other.bg_r, other.bg_g, other.bg_b, other.style) { }

        explicit CharStyle(CharStyle&& other) noexcept :
            CharStyle(other.fg_color, other.fg_ansi, other.fg_r, other.fg_g, other.fg_b,
                other.bg_color, other.bg_ansi, other.bg_r, other.bg_g, other.bg_b, other.style) {
            other.reset();
        }

        constexpr CharStyle()
            : CharStyle(Color::BLACK, 0, 0, 0, 0, Color::WHITE, 0, 0, 0, 0, StyleFlags::NONE) { }

        constexpr CharStyle(Color fg, Color bg, StyleFlags style = StyleFlags::NONE)
            : CharStyle(fg, 0, 0, 0, 0, bg, 0, 0, 0, 0, style) { }

        constexpr CharStyle(int fg, int bg, StyleFlags style = StyleFlags::NONE)
            : CharStyle(Color::ANSI8, fg, 0, 0, 0, Color::ANSI8, bg, 0, 0, 0, style) { }

        constexpr CharStyle(int fr, int fg, int fb, int br, int bg, int bb, StyleFlags style = StyleFlags::NONE)
            : CharStyle(Color::RGB, 0, fr, fg, fb, Color::RGB, 0, br, bg, bb, style) { }

        // Операторы сравнения
        constexpr bool operator==(const CharStyle& other) const noexcept {
            if (fg_color != other.fg_color)
                return false;
            if (bg_color != other.bg_color)
                return false;
            if (style != other.style)
                return false;
            if (fg_ansi != other.fg_ansi || bg_ansi != other.bg_ansi)
                return false;
            return fg_r == other.fg_r && fg_g == other.fg_g && fg_b == other.fg_b &&
                bg_r == other.bg_r && bg_g == other.bg_g && bg_b == other.bg_b;
        }

        constexpr bool operator!=(const CharStyle& other) const noexcept {
            return !(*this == other);
        }

        CharStyle& operator=(const CharStyle& other) noexcept {
            if (*this == other)
                return *this;
            bg_color = other.bg_color;
            fg_color = other.fg_color;
            bg_ansi = other.bg_ansi;
            fg_ansi = other.fg_ansi;
            bg_r = other.bg_r;
            bg_g = other.bg_g;
            bg_b = other.bg_b;
            fg_r = other.fg_r;
            fg_g = other.fg_g;
            fg_b = other.fg_b;
            style = other.style;
        }


        // Валидация
        Result<void> validate() const noexcept {
            if (fg_color == Color::RGB) {
                if (fg_r < 0 || fg_r > 255 || fg_g < 0 || fg_g > 255 || fg_b < 0 || fg_b > 255) {
                    return Err(ErrorCode::VALIDATION_FAILED, "Foreground RGB values must be in range [0, 255]");
                }
            }
            if (bg_color == Color::RGB) {
                if (bg_r < 0 || bg_r > 255 || bg_g < 0 || bg_g > 255 || bg_b < 0 || bg_b > 255) {
                    return Err(ErrorCode::VALIDATION_FAILED, "Background RGB values must be in range [0, 255]");
                }
            }
            if (fg_color == Color::ANSI8 && (fg_ansi < 0 || fg_ansi > 255)) {
                return Err(ErrorCode::VALIDATION_FAILED, "Foreground ANSI8 value must be in range [0, 255]");
            }
            if (bg_color == Color::ANSI8 && (bg_ansi < 0 || bg_ansi > 255)) {
                return Err(ErrorCode::VALIDATION_FAILED, "Background ANSI8 value must be in range [0, 255]");
            }
            return Ok();
        }

        bool is_valid() const noexcept {
            return validate().is_ok();
        }

        void reset() {
            fg_color = Color::BLACK;
            bg_color = Color::WHITE;
            fg_ansi = fg_r = fg_g = fg_b = 0;
            bg_ansi = bg_r = bg_g = bg_b = 0;
            style = StyleFlags::NONE;
        }
    };


    class CharStyleStringBuilder {
        CharStyle value_;

        static int color_to_ansi_fg(Color color) noexcept {
            switch (color) {
            case Color::BLACK: return 30;
            case Color::RED: return 31;
            case Color::GREEN: return 32;
            case Color::YELLOW: return 33;
            case Color::BLUE: return 34;
            case Color::MAGENTA: return 35;
            case Color::CYAN: return 36;
            case Color::WHITE: return 37;
            case Color::GREY: return 90;
            case Color::BRIGHT_RED: return 91;
            case Color::BRIGHT_GREEN: return 92;
            case Color::BRIGHT_YELLOW: return 93;
            case Color::BRIGHT_BLUE: return 94;
            case Color::BRIGHT_MAGENTA: return 95;
            case Color::BRIGHT_CYAN: return 96;
            case Color::BRIGHT_WHITE: return 97;
            default: return 37;
            }
        }

        static int color_to_ansi_bg(Color color) noexcept {
            switch (color) {
            case Color::BLACK: return 40;
            case Color::RED: return 41;
            case Color::GREEN: return 42;
            case Color::YELLOW: return 43;
            case Color::BLUE: return 44;
            case Color::MAGENTA: return 45;
            case Color::CYAN: return 46;
            case Color::WHITE: return 47;
            case Color::GREY: return 100;
            case Color::BRIGHT_RED: return 101;
            case Color::BRIGHT_GREEN: return 102;
            case Color::BRIGHT_YELLOW: return 103;
            case Color::BRIGHT_BLUE: return 104;
            case Color::BRIGHT_MAGENTA: return 105;
            case Color::BRIGHT_CYAN: return 106;
            case Color::BRIGHT_WHITE: return 107;
            default: return 40;
            }
        }


    public:
        CharStyleStringBuilder() noexcept {
            value_ = CharStyle();
        }
        explicit CharStyleStringBuilder(const CharStyle& style) : value_(style) {}

        const CharStyle& get() const noexcept {
            return value_;
        }

        std::string to_string() const {
            std::ostringstream oss;
            oss << "\033[";
            bool need_separator = false;
            // Формируем стиль текста
            if (has_style(static_cast<StyleFlags>(value_.style), StyleFlags::BOLD)) {
                oss << "1";
                need_separator = true;
            }
            if (has_style(static_cast<StyleFlags>(value_.style), StyleFlags::DIM)) {
                if (need_separator) oss << ";";
                oss << "2";
                need_separator = true;
            }
            if (has_style(static_cast<StyleFlags>(value_.style), StyleFlags::ITALIC)) {
                if (need_separator) oss << ";";
                oss << "3";
                need_separator = true;
            }
            if (has_style(static_cast<StyleFlags>(value_.style), StyleFlags::UNDERLINED)) {
                if (need_separator) oss << ";";
                oss << "4";
                need_separator = true;
            }
            if (has_style(static_cast<StyleFlags>(value_.style), StyleFlags::BLINK_SLOW)) {
                if (need_separator) oss << ";";
                oss << "5";
                need_separator = true;
            }
            if (has_style(static_cast<StyleFlags>(value_.style), StyleFlags::BLINK_FAST)) {
                if (need_separator) oss << ";";
                oss << "6";
                need_separator = true;
            }
            if (has_style(static_cast<StyleFlags>(value_.style), StyleFlags::INVERSE)) {
                if (need_separator) oss << ";";
                oss << "7";
                need_separator = true;
            }
            if (has_style(static_cast<StyleFlags>(value_.style), StyleFlags::HIDDEN)) {
                if (need_separator) oss << ";";
                oss << "8";
                need_separator = true;
            }
            if (has_style(static_cast<StyleFlags>(value_.style), StyleFlags::STRIKE_THROUGH)) {
                if (need_separator) oss << ";";
                oss << "9";
                need_separator = true;
            }
            // Формируем цвет текста
            if (value_.fg_color == Color::RGB) {
                if (need_separator) oss << ";";
                oss << "38;2;" << value_.fg_r << ";" << value_.fg_g << ";" << value_.fg_b;
                need_separator = true;
            }
            else if (value_.fg_color == Color::ANSI8) {
                if (need_separator) oss << ";";
                oss << "38;5;" << value_.fg_ansi;
                need_separator = true;
            }
            else {
                if (need_separator) oss << ";";
                oss << color_to_ansi_fg(value_.fg_color);
                need_separator = true;
            }
            // Формируем цвет фона
            if (value_.bg_color == Color::RGB) {
                if (need_separator) oss << ";";
                oss << "48;2;" << value_.bg_r << ";" << value_.bg_g << ";" << value_.bg_b;
            }
            else if (value_.bg_color == Color::ANSI8) {
                if (need_separator) oss << ";";
                oss << "48;5;" << value_.bg_ansi;
            }
            else {
                if (need_separator) oss << ";";
                oss << color_to_ansi_bg(value_.bg_color);
            }
            oss << "m";
            return oss.str();
        }
    };

}