#pragma once


#include "RawCharMatrix.h"
#include "StyleMatrix.h"
#include "char_styling.h"
#include "StyleStack.h"


namespace dungeons::tui {


    class FrameSnapshot {
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