#pragma once


#include "tui_matrixes.h"
#include "char_styling.h"
#include "StyleStack.h"


namespace dungeons::tui {


    class StylePicker {
        StyleStack styles_;
        CharStyle current_;
        size_t current_repeats_;

        void push_style(const CharStyle& value) {
            if (value == current_) {
                current_repeats_++;
                return;
            }
            styles_.push(current_);
            current_ = value;
        }

        Result<void> pop_style() {
            if (current_repeats_ > 0) {
                current_repeats_--;
                return Ok();
            }
            return styles_.pop();
        }

    public:
        StylePicker() = default;
        explicit StylePicker(const CharStyle& initial_style) : current_(initial_style), current_repeats_{0} {}


        Result<void> apply_style(const CharStyle new_style, StyleOperation operations = StyleOperation::NONE) {
            CharStyle style = CharStyle(current_);
            #ifdef DNG_EXPERIMENTAL // Функционал с частичным применением стилей не реализован до конца. Поэтому отмечен как экспериментальный.
            if (has_operation(operations, StyleOperation::RESET_STYLE)) {
                reset();
                return Ok();
            }
            if (has_operation(operations, StyleOperation::SET_FG_COLOR)) {
                style.fg_color = new_style.fg_color;
                style.fg_ansi = new_style.fg_ansi;
                style.fg_r = new_style.fg_r;
                style.fg_g = new_style.fg_g;
                style.fg_b = new_style.fg_b;
            }
            if (has_operation(operations, StyleOperation::SET_BG_COLOR)) {
                style.bg_color = new_style.bg_color;
                style.bg_ansi = new_style.bg_ansi;
                style.bg_r = new_style.bg_r;
                style.bg_g = new_style.bg_g;
                style.bg_b = new_style.bg_b;
            }
            if (has_operation(operations, StyleOperation::SET_STYLE)) {
                style.style = new_style.style;
            }
            #else
            style.fg_color = new_style.fg_color;
            style.fg_ansi = new_style.fg_ansi;
            style.fg_r = new_style.fg_r;
            style.fg_g = new_style.fg_g;
            style.fg_b = new_style.fg_b;
            style.bg_color = new_style.bg_color;
            style.bg_ansi = new_style.bg_ansi;
            style.bg_r = new_style.bg_r;
            style.bg_g = new_style.bg_g;
            style.bg_b = new_style.bg_b;
            style.style = new_style.style;
            #endif
            push_style(style);
            return Ok();
        }

        const CharStyle& current() const noexcept {
            return current_;
        }

        std::string current_as_string() const {
            return CharStyleStringBuilder(current_).to_string();
        }

        Result<void> reset() {
            auto peek_res = styles_.peek_basic();
            if (!peek_res)
                return Result<void>(peek_res.error());
            current_ = peek_res.value();
            styles_.clear();
        }

        size_t size() const noexcept {
            return styles_.size();
        }
    };


    class FrameSnapshot {
        StylePicker picker_;
        size_t rows_;
        size_t cols_;

        Result<void> validate_position(size_t row, size_t col) const noexcept {
            if (row >= rows_) {
                return Err(ErrorCode::OUT_OF_RANGE,
                    "Row " + std::to_string(row) + " is out of range [0, " + std::to_string(rows_) + ")");
            }
            if (col >= cols_) {
                return Err(ErrorCode::OUT_OF_RANGE,
                    "Column " + std::to_string(col) + " is out of range [0, " + std::to_string(cols_) + ")");
            }
            return Ok();
        }


    public:
        // Конструкторы
        


        // Методы доступа
        Result<void> set_chars(std::initializer_list<std::string_view> lines) {
            return Err();
        }


        std::string to_string() const {
            std::ostringstream result;
            
            return result.str();
        }


        // Доступ к символу для чтения. Возвращает копию.
        Result<char> get_char(size_t row, size_t col) const {
            // validation
            // get
            // return code and char
            return Err<char>();
        }

        Result<void> set_char(size_t row, size_t col, const char value) {
            // validation
            // set
            // return code
            return Err();
        }

        Result<void> set_char(size_t row, size_t col, char value) {
            // validation
            // set
            // return code
            return Err();
        }

        Result<void> set_default_style(const CharStyle& style) {
            // validation?
            // set
            // return code
            return Err();
        }

        Result<const CharStyle&> get_default_style() const noexcept {
            return Err<const CharStyle&>();
        }

        constexpr size_t rows() const noexcept { return rows_; }
        constexpr size_t cols() const noexcept { return cols_; }
    };
}