#pragma once


#include "char_styling.h"
#include "RawCharMatrix.h"
#include "UnicodeCharMatrix.h"
#include "StyleStack.h"
#include "StyleMatrix.h"


namespace dungeons::tui {


    /**
     FrameCursor - структура для отслеживания состояния при рендеринге
     Хранит текущую позицию и стиль во время парсинга фрейма
     */
    struct FrameCursor {
        CharStyle prev;
        CharStyle cur;
        size_t row;
        size_t col;
    };


    /**
     FrameSnapshot - снимок кадра с символами и стилями
     
     Представляет собой 2D сетку символов (Unicode) со стилями для каждого символа.
     Используется для композиции UI элементов перед рендерингом в терминал.
     
     Основные возможности:
     - Работа с Unicode символами (кириллица, эмодзи и т.д.)
     - Индивидуальное стилирование каждого символа
     - Копирование областей между фреймами
     - Конвертация в ANSI строку для вывода
     */
    class FrameSnapshot {
        UnicodeCharMatrix chars_; // Матрица символов (UTF-32)
        StyleMatrix styles_; // Матрица стилей
        size_t rows_; // Количество строк
        size_t cols_; // Количество столбцов

        // ============================================================================
        // Валидация
        // ============================================================================

        Result<void> validate_not_empty() const noexcept {
            if (rows_ == 0 || cols_ == 0) {
                return Err(ErrorCode::EMPTY_CONTAINER, "Frame is empty");
            }
            return Ok();
        }

        Result<void> validate_position(size_t row, size_t col) const noexcept {
            auto check = validate_not_empty();
            if (!check) return check;

            if (row >= rows_) {
                return Err(ErrorCode::OUT_OF_RANGE,
                    "Row " + std::to_string(row) + " is out of range [0, " +
                    std::to_string(rows_) + ")");
            }

            if (col >= cols_) {
                return Err(ErrorCode::OUT_OF_RANGE,
                    "Column " + std::to_string(col) + " is out of range [0, " +
                    std::to_string(cols_) + ")");
            }

            return Ok();
        }

        Result<void> validate_range(size_t start_row, size_t start_col,
            size_t end_row, size_t end_col) const noexcept {
            if (start_row > end_row) {
                return Err(ErrorCode::INVALID_ARGUMENT,
                    "start_row (" + std::to_string(start_row) +
                    ") must be <= end_row (" + std::to_string(end_row) + ")");
            }

            if (start_col > end_col) {
                return Err(ErrorCode::INVALID_ARGUMENT,
                    "start_col (" + std::to_string(start_col) +
                    ") must be <= end_col (" + std::to_string(end_col) + ")");
            }

            auto start_check = validate_position(start_row, start_col);
            if (!start_check) return start_check;

            auto end_check = validate_position(end_row, end_col);
            if (!end_check) return end_check;

            return Ok();
        }


    public:
        // ============================================================================
        // Конструкторы
        // ============================================================================

        // Создать фрейм из готовой матрицы символов
        FrameSnapshot(UnicodeCharMatrix chars, CharStyle defaultStyle) : chars_{ chars }, 
            rows_{ chars.rows() }, cols_{ chars.cols().value() }, 
            styles_ { StyleMatrix(defaultStyle, chars.rows(), chars.cols().value()) } {}
        
        // Создать фрейм заданного размера, заполненный одним и тем же символом
        FrameSnapshot(char32_t filler, CharStyle defaultStyle, size_t rows, size_t cols) :
            chars_{ filler, rows, cols }, styles_{ defaultStyle, rows, cols },
            rows_{ rows }, cols_{ cols } {}


        // ============================================================================
        // Работа с символами - массовая установка
        // ============================================================================

        // Заменить всю матрицу символов (размеры должны совпадать)
        Result<void> set_chars(const std::initializer_list<std::string_view>& lines) {
            try {
                UnicodeCharMatrix new_chars(lines);
                if (new_chars.rows() != rows_ || new_chars.cols().value() != cols_) {
                    return Err(ErrorCode::INVALID_ARGUMENT,
                        "New matrix size (" + std::to_string(new_chars.rows()) + "x" +
                        std::to_string(new_chars.cols().value()) +
                        ") doesn't match frame size (" + std::to_string(rows_) + "x" +
                        std::to_string(cols_) + ")");
                }
                chars_ = std::move(new_chars);
                return Ok();
            }
            catch (const std::exception& e) {
                return Err(ErrorCode::EXCEPTION, std::string("Failed to set chars: ") + e.what());
            }
        }

        // Установить строку символов (UTF-8)
        Result<void> set_char_row(size_t row, const std::string& utf8_str) {
            return chars_.set_row(row, utf8_str);
        }

        // Получить строку символов как UTF-8
        Result<std::string> get_char_row(size_t row) const {
            return chars_.get_row(row);
        }

        // ============================================================================
        // Работа с отдельными символами
        // ============================================================================

        // Получить символ (char32_t)
        Result<char32_t> get_char(size_t row, size_t col) const {
            return chars_.get_at(row, col);
        }

        // Получить символ как UTF-8 строку
        Result<std::string> get_char_utf8(size_t row, size_t col) const {
            return chars_.get_at_utf8(row, col);
        }

        // Установить символ (char32_t)
        Result<void> set_char(size_t row, size_t col, char32_t value) {
            return chars_.set_at(row, col, value);
        }

        // Установить символ из UTF-8 строки
        Result<void> set_char_utf8(size_t row, size_t col, const std::string& utf8_char) {
            return chars_.set_at_utf8(row, col, utf8_char);
        }


        // ============================================================================
        // Работа со стилями - области
        // ============================================================================

        // Установить стиль для прямоугольной области
        Result<void> set_area_style(size_t start_row, size_t start_col,
            size_t end_row, size_t end_col, CharStyle style) {
            return styles_.set_area(start_row, start_col, end_row, end_col, style);
        }

        // Установить стиль для целой строки
        Result<void> set_row_style(size_t row, CharStyle style) {
            return styles_.set_row(row, style);
        }

        // Установить стиль для отдельного символа
        Result<void> set_style(size_t row, size_t col, CharStyle style) {
            return styles_.set_at(row, col, style);
        }

        // Получить стиль символа
        Result<CharStyle> style_at(size_t row, size_t col) const  {
            return styles_.at(row, col);
        }

        Result<CharStyle> default_style() const noexcept {
            return styles_.default_style();
        }
        
        Result<void> set_default_style(const CharStyle& style) {
            return styles_.set_default_style(style);
        }


        constexpr size_t rows() const noexcept { return rows_; }
        constexpr size_t cols() const noexcept { return cols_; }
        
        
        Result<std::string> to_string() const {
            std::ostringstream result;
            FrameCursor cursor;
            cursor.row = cursor.col = 0;
            auto& row = cursor.row;
            auto& col = cursor.col;
            auto style_res = style_at(row, col);
            while (row < rows_) {
                while (col < cols_) {
                    cursor.prev = cursor.cur;
                    if (!(style_res = style_at(row, col)))
                        return Result<std::string>(style_res.error());
                    cursor.cur = style_res.value();
                    if (cursor.prev != cursor.cur) {
                        result << CharStyleStringBuilder(cursor.cur).to_string(); // Заменить на поток?
                    }
                    result << UnicodeCharMatrix::utf32_to_utf8(std::u32string(1, chars_.get_at(row, col).value())); // Страшно.
                    col++;
                }
                result << '\n';
                row++;
                col = 0;
            }
            return result.str();
        }


        // ============================================================================
        // Работа с символами и стилями одновременно
        // ============================================================================

        /**
         * Установить символ и стиль одновременно
         */
        Result<void> set_cell(size_t row, size_t col, char32_t ch, CharStyle style) {
            auto char_result = set_char(row, col, ch);
            if (!char_result) return char_result;
            auto style_result = set_style(row, col, style);
            if (!style_result) return style_result;
            return Ok();
        }

        /**
         * Установить символ и стиль одновременно (UTF-8 версия)
         */
        Result<void> set_cell_utf8(size_t row, size_t col,
            const std::string& utf8_char, CharStyle style) {
            auto char_result = set_char_utf8(row, col, utf8_char);
            if (!char_result) return char_result;
            auto style_result = set_style(row, col, style);
            if (!style_result) return style_result;
            return Ok();
        }


        // ============================================================================
        // Работа с областями - копирование и вставка
        // ============================================================================

        /**
         * Скопировать область из другого фрейма в этот
         *
         * @param source - исходный фрейм
         * @param src_row, src_col - начало в исходном фрейме
         * @param dst_row, dst_col - начало в целевом фрейме (this)
         * @param height, width - размеры копируемой области
         */
        Result<void> blit(const FrameSnapshot& source,
            size_t src_row, size_t src_col,
            size_t dst_row, size_t dst_col,
            size_t height, size_t width) {
            // Валидация исходной области
            if (src_row + height > source.rows_ || src_col + width > source.cols_) {
                return Err(ErrorCode::OUT_OF_RANGE, "Source region out of bounds");
            }
            // Валидация целевой области
            if (dst_row + height > rows_ || dst_col + width > cols_) {
                return Err(ErrorCode::OUT_OF_RANGE, "Destination region out of bounds");
            }
            // Копируем символы и стили
            for (size_t i = 0; i < height; ++i) {
                for (size_t j = 0; j < width; ++j) {
                    auto ch_result = source.get_char(src_row + i, src_col + j);
                    if (!ch_result) return Result<void>(ch_result.error());

                    auto style_result = source.style_at(src_row + i, src_col + j);
                    if (!style_result) return Result<void>(style_result.error());

                    auto set_result = set_cell(dst_row + i, dst_col + j,
                        ch_result.value(), style_result.value());
                    if (!set_result) return set_result;
                }
            }
            return Ok();
        }

        /**
         * Скопировать область внутри одного фрейма
         */
        Result<void> copy_region(size_t src_row, size_t src_col,
            size_t dst_row, size_t dst_col,
            size_t height, size_t width) {
            // Создаем временный буфер для избежания перезаписи при перекрытии
            auto char_sub = chars_.submatrix(src_row, src_col, src_row + height - 1, src_col + width - 1);
            if (!char_sub) 
                return Result<void>(char_sub.error());
            auto style_sub = styles_.submatrix(src_row, src_col, src_row + height - 1, src_col + width - 1);
            if (!style_sub) 
                return Result<void>(style_sub.error());
            // Вставляем во временный фрейм
            FrameSnapshot temp(char_sub.value(), default_style().value());
            auto style_set = temp.styles_.set_submatrix(0, 0, style_sub.value());
            if (!style_set) 
                return style_set;
            // Копируем из временного фрейма
            return blit(temp, 0, 0, dst_row, dst_col, height, width);
        }


        // ============================================================================
        // Очистка и заполнение
        // ============================================================================

        // Очистить весь фрейм (символы и стили)
        void clear() {
            chars_.clear();  // Заполняет пробелами
            styles_.clear(); // Заполняет default_style_
        }

        // Очистить область
        Result<void> clear_region(size_t start_row, size_t start_col,
            size_t end_row, size_t end_col) {
            auto validation = validate_range(start_row, start_col, end_row, end_col);
            if (!validation) 
                return validation;
            for (size_t i = start_row; i <= end_row; ++i) {
                for (size_t j = start_col; j <= end_col; ++j) {
                    auto char_result = chars_.set_at(i, j, U' ');
                    if (!char_result) return char_result;
                }
            }
            return styles_.set_area(start_row, start_col, end_row, end_col, default_style().value());
        }

        // Заполнить область символом и стилем
        Result<void> fill_region(size_t start_row, size_t start_col,
            size_t end_row, size_t end_col,
            char32_t ch, CharStyle style) {
            auto validation = validate_range(start_row, start_col, end_row, end_col);
            if (!validation) return validation;
            for (size_t i = start_row; i <= end_row; ++i) {
                for (size_t j = start_col; j <= end_col; ++j) {
                    auto char_result = chars_.set_at(i, j, ch);
                    if (!char_result) return char_result;
                }
            }
            return styles_.set_area(start_row, start_col, end_row, end_col, style);
        }


        // ============================================================================
        // Дебаг и информация
        // ============================================================================

        // Получить дамп фрейма без ANSI кодов (для отладки)
        std::string to_plain_string() const {
            return chars_.to_string();
        }

        // Получить информацию о фрейме
        std::string info() const {
            std::ostringstream oss;
            oss << "FrameSnapshot: " << rows_ << "x" << cols_ << " cells\n";
            oss << "UTF-8 byte size: " << chars_.utf8_byte_size() << " bytes\n";
            oss << "Default style: fg=" << static_cast<int>(default_style().value().fg_color)
                << " bg=" << static_cast<int>(default_style().value().bg_color) << "\n";
            return oss.str();
        }
    };


}