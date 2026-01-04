#pragma once


#include "tui_matrixes.h"
#include "char_styling.h"


namespace dungeons::tui {


    class StyleApplicator {
        std::stack<CharStyle> style_stack_;
        CharStyle current_style_;

    public:
        StyleApplicator() = default;
        explicit StyleApplicator(const CharStyle& initial_style) : current_style_(initial_style) {}

        void push_style() {
            style_stack_.push(current_style_);
        }

        Result<void> pop_style() {
            if (style_stack_.empty()) {
                return Err(ErrorCode::EMPTY_CONTAINER, "Cannot pop from empty style stack");
            }
            current_style_ = style_stack_.top();
            style_stack_.pop();
            return Ok();
        }

        Result<void> pop_styles(size_t count) {
            if (count > style_stack_.size()) {
                return Err(ErrorCode::OUT_OF_RANGE,
                    "Cannot pop " + std::to_string(count) + " styles, only " +
                    std::to_string(style_stack_.size()) + " available");
            }
            for (size_t i = 0; i < count; ++i) {
                current_style_ = style_stack_.top();
                style_stack_.pop();
            }
            return Ok();
        }

        Result<void> apply_operations(StyleOperation operations,
            const CharStyle new_style) {
            CharStyleBuilder builder(current_style_);
            if (has_operation(operations, StyleOperation::SET_FG_COLOR)) {
                builder.set_fg_color(new_style.fg_color)
                    .set_fg_ansi(new_style.fg_ansi)
                    .set_fg_rgb(new_style.fg_r, new_style.fg_g, new_style.fg_b);
            }
            if (has_operation(operations, StyleOperation::SET_BG_COLOR)) {
                builder.set_bg_color(new_style.bg_color)
                    .set_bg_ansi(new_style.bg_ansi)
                    .set_bg_rgb(new_style.bg_r, new_style.bg_g, new_style.bg_b);
            }
            if (has_operation(operations, StyleOperation::SET_STYLE)) {
                builder.set_style(new_style.fg_style);
            }
            if (has_operation(operations, StyleOperation::RESET_STYLE)) {
                auto pop_result = pop_style();
                if (!pop_result)
                    return pop_result;
            }
            else {
                auto result = builder.build();
                if (!result) {
                    return Err(result.error().code(), result.error().message());
                }  
                current_style_ = result.value();
            }
            return Ok();
        }

        const CharStyle& current_style() const noexcept {
            return current_style_;
        }

        std::string get_style_string() const {
            return CharStyleBuilder(current_style_).to_string();
        }

        void reset() noexcept {
            current_style_ = CharStyle();
            while (!style_stack_.empty()) {
                style_stack_.pop();
            }
        }

        size_t stack_size() const noexcept {
            return style_stack_.size();
        }
    };


    class FrameSnapshot {
        inline static const FrameChar fill_ = FrameChar(' ');
        std::vector<std::vector<FrameChar>> chars_;
        StyleApplicator applicator_;
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
            if (lines.size() > rows_) {
                return Err(ErrorCode::INVALID_ARGUMENT,
                    "Number of lines (" + std::to_string(lines.size()) +
                    ") exceeds number of rows (" + std::to_string(rows_) + ")");
            }
            size_t i = 0;
            for (const auto& line : lines) {
                if (line.size() > cols_) {
                    std::fill(chars_[i].begin(), chars_[i].end(), fill_);
                }
                else {
                    for (size_t j = 0; j < line.size(); j++) {
                        chars_[i][j].value(line[j]);
                    }
                    std::fill(chars_[i].begin() + line.size(), chars_[i].end(), fill_);
                }
                i++;
            }
            for (; i < rows_; i++) {
                std::fill(chars_[i].begin(), chars_[i].end(), fill_);
            }
            return Ok();
        }


        std::string to_string() const {
            std::ostringstream result;
            for (size_t i = 0; i < rows_; i++) {
                for (size_t j = 0; j < cols_; j++) {
                    const auto& ch = chars_[i][j];
                    if (ch.has_operations()) {
                        applicator.push_style();
                        // Применяем все операции символа
                        for (const auto& op : ch.operations()) {
                            
                        }
                    }
                    result << ch.value();
                    if (ch.has_operations()) {
                        auto pop_result = applicator.pop_style();
                        if (pop_result) {
                            result << applicator.get_style_string();
                        }
                    }
                }
                result << '\n';
            }
            return result.str();
        }


        // Доступ к символу для чтения. Возвращает копию.
        Result<FrameChar> get_char(size_t row, size_t col) const {
            auto validation = validate_position(row, col);
            if (!validation) {
                return Err<FrameChar>(validation.error().code(), validation.error().message());
            }
            FrameChar res = FrameChar(chars_[row][col]);
            return Ok(res);
        }

        Result<void> set_char(size_t row, size_t col, const FrameChar& value) {
            auto validation = validate_position(row, col);
            if (!validation) {
                return validation;
            }
            chars_[row][col] = value;
            return Ok();
        }

        Result<void> set_char(size_t row, size_t col, char value) {
            auto validation = validate_position(row, col);
            if (!validation) {
                return validation;
            }
            chars_[row][col].value(value);
            return Ok();
        }

        Result<void> set_default_style(const CharStyle& style) {
            auto validation = style.validate();
            if (!validation) {
                return validation;
            }
            default_style_ = style;
            return Ok();
        }

        const CharStyle& get_default_style() const noexcept {
            return default_style_;
        }

        constexpr size_t rows() const noexcept { return rows_; }
        constexpr size_t cols() const noexcept { return cols_; }
    };
}