#pragma once

#include "tui_base.h"

namespace dungeons::tui {

    /**
     UnicodeCharMatrix - матрица Unicode символов (поддержка UTF-8)
     
     В отличие от RawCharMatrix, которая работает с байтами,
     этот класс корректно обрабатывает многобайтовые UTF-8 символы,
     включая кириллицу, эмодзи и другие Unicode символы.
     
     Пример:
       RawCharMatrix("АБВ")    -> 6 столбцов (6 байт)
       UnicodeCharMatrix("АБВ") -> 3 столбца (3 символа)
     */
    class UnicodeCharMatrix {
        std::vector<std::vector<char32_t>> container_;


        // ============================================================================
        // Вспомогательные методы валидации (идентичны RawCharMatrix)
        // ============================================================================

        Result<void> validate_not_empty() const noexcept {
            if (container_.empty()) {
                return Err(ErrorCode::EMPTY_CONTAINER, "Matrix is empty");
            }
            return Ok();
        }

        Result<void> validate_row(size_t row) const noexcept {
            auto check = validate_not_empty();
            if (!check) 
                return check;
            if (row >= container_.size()) {
                return Err(ErrorCode::OUT_OF_RANGE,
                    "Row " + std::to_string(row) + " is out of range [0, " +
                    std::to_string(container_.size()) + ")");
            }
            return Ok();
        }

        Result<void> validate_col(size_t col) const noexcept {
            auto check = validate_not_empty();
            if (!check) 
                return check;
            if (col >= container_[0].size()) {
                return Err(ErrorCode::OUT_OF_RANGE,
                    "Column " + std::to_string(col) + " is out of range [0, " +
                    std::to_string(container_[0].size()) + ")");
            }
            return Ok();
        }

        Result<void> validate_position(size_t row, size_t col) const noexcept {
            auto row_check = validate_row(row);
            if (!row_check) 
                return row_check;
            auto col_check = validate_col(col);
            if (!col_check) 
                return col_check;
            return Ok();
        }

        Result<void> validate_row_range(size_t start_row, size_t end_row) const noexcept {
            if (start_row > end_row) {
                return Err(ErrorCode::INVALID_ARGUMENT,
                    "start_row (" + std::to_string(start_row) +
                    ") must be <= end_row (" + std::to_string(end_row) + ")");
            }
            auto check = validate_row(end_row);
            if (!check) return check;
            return Ok();
        }

        Result<void> validate_col_range(size_t start_col, size_t end_col) const noexcept {
            if (start_col > end_col) {
                return Err(ErrorCode::INVALID_ARGUMENT,
                    "start_col (" + std::to_string(start_col) +
                    ") must be <= end_col (" + std::to_string(end_col) + ")");
            }
            auto check = validate_col(end_col);
            if (!check) return check;
            return Ok();
        }

        Result<void> validate_range(size_t start_row, size_t start_col,
            size_t end_row, size_t end_col) const noexcept {
            auto row_check = validate_row_range(start_row, end_row);
            if (!row_check) return row_check;
            auto col_check = validate_col_range(start_col, end_col);
            if (!col_check) return col_check;
            return Ok();
        }

        Result<void> validate_size_matches_cols(size_t size) const noexcept {
            auto check = validate_not_empty();
            if (!check) return check;
            size_t col_count = container_[0].size();
            if (size != col_count) {
                return Err(ErrorCode::INVALID_ARGUMENT,
                    "Size (" + std::to_string(size) +
                    ") doesn't match column count (" + std::to_string(col_count) + ")");
            }
            return Ok();
        }

        Result<void> validate_submatrix_fits(size_t start_row, size_t start_col,
            size_t sub_rows, size_t sub_cols) const noexcept {
            auto pos_check = validate_position(start_row, start_col);
            if (!pos_check) return pos_check;
            size_t end_row = start_row + sub_rows - 1;
            size_t end_col = start_col + sub_cols - 1;
            if (end_row >= container_.size()) {
                return Err(ErrorCode::OUT_OF_RANGE,
                    "Submatrix doesn't fit: end_row " + std::to_string(end_row) +
                    " >= matrix rows " + std::to_string(container_.size()));
            }
            if (end_col >= container_[0].size()) {
                return Err(ErrorCode::OUT_OF_RANGE,
                    "Submatrix doesn't fit: end_col " + std::to_string(end_col) +
                    " >= matrix cols " + std::to_string(container_[0].size()));
            }
            return Ok();
        }

    public:
        // ============================================================================
        // Вспомогательные методы конвертации UTF-8 <-> UTF-32
        // ============================================================================

        /**
         Конвертация UTF-8 строки в UTF-32
         Каждый UTF-8 символ (1-4 байта) преобразуется в один char32_t
         */
        static std::u32string utf8_to_utf32(const std::string& utf8) {
            std::u32string result;
            result.reserve(utf8.size()); // Оптимизация: предполагаем ASCII
            for (size_t i = 0; i < utf8.size(); ) {
                unsigned char c = static_cast<unsigned char>(utf8[i]);
                char32_t unicode_char = 0;
                if ((c & 0x80) == 0) {
                    // 1-байтовый символ (ASCII): 0xxxxxxx
                    unicode_char = c;
                    i += 1;
                }
                else if ((c & 0xE0) == 0xC0) {
                    // 2-байтовый символ: 110xxxxx 10xxxxxx
                    if (i + 1 >= utf8.size()) break;
                    unicode_char = ((c & 0x1F) << 6) |
                        (static_cast<unsigned char>(utf8[i + 1]) & 0x3F);
                    i += 2;
                }
                else if ((c & 0xF0) == 0xE0) {
                    // 3-байтовый символ: 1110xxxx 10xxxxxx 10xxxxxx
                    if (i + 2 >= utf8.size()) break;
                    unicode_char = ((c & 0x0F) << 12) |
                        ((static_cast<unsigned char>(utf8[i + 1]) & 0x3F) << 6) |
                        (static_cast<unsigned char>(utf8[i + 2]) & 0x3F);
                    i += 3;
                }
                else if ((c & 0xF8) == 0xF0) {
                    // 4-байтовый символ: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                    if (i + 3 >= utf8.size()) break;
                    unicode_char = ((c & 0x07) << 18) |
                        ((static_cast<unsigned char>(utf8[i + 1]) & 0x3F) << 12) |
                        ((static_cast<unsigned char>(utf8[i + 2]) & 0x3F) << 6) |
                        (static_cast<unsigned char>(utf8[i + 3]) & 0x3F);
                    i += 4;
                }
                else {
                    // Некорректный UTF-8, пропускаем байт
                    i++;
                    continue;
                }
                result.push_back(unicode_char);
            }
            return result;
        }

        /**
         Конвертация UTF-32 строки в UTF-8
         Каждый char32_t преобразуется в 1-4 байта UTF-8
         */
        static std::string utf32_to_utf8(const std::u32string& utf32) {
            std::string result;
            result.reserve(utf32.size() * 2); // Оптимизация: предполагаем ~2 байта на символ
            for (char32_t ch : utf32) {
                if (ch < 0x80) {
                    // 1-байтовый символ (ASCII)
                    result.push_back(static_cast<char>(ch));
                }
                else if (ch < 0x800) {
                    // 2-байтовый символ
                    result.push_back(static_cast<char>(0xC0 | (ch >> 6)));
                    result.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
                }
                else if (ch < 0x10000) {
                    // 3-байтовый символ
                    result.push_back(static_cast<char>(0xE0 | (ch >> 12)));
                    result.push_back(static_cast<char>(0x80 | ((ch >> 6) & 0x3F)));
                    result.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
                }
                else if (ch < 0x110000) {
                    // 4-байтовый символ
                    result.push_back(static_cast<char>(0xF0 | (ch >> 18)));
                    result.push_back(static_cast<char>(0x80 | ((ch >> 12) & 0x3F)));
                    result.push_back(static_cast<char>(0x80 | ((ch >> 6) & 0x3F)));
                    result.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
                }
                else {
                    // Некорректный Unicode код, пропускаем. TODO: новый тип ошибки?
                    continue;
                }
            }
            return result;
        }

        // ============================================================================
        // Конструкторы
        // ============================================================================

        /**
         * Конструктор с заполнением Unicode символом
         * @param filler - Unicode символ для заполнения (char32_t)
         * @param rows - количество строк
         * @param cols - количество столбцов
         */
        UnicodeCharMatrix(char32_t filler, size_t rows, size_t cols)
            : container_(rows, std::vector<char32_t>(cols, filler)) {
            if (rows == 0 || cols == 0) {
                throw std::invalid_argument("Rows and cols must be greater than 0");
            }
        }

        /**
         Конструктор из UTF-8 строки с явными размерами
         Разбивает строку на rows строк по cols символов каждая
         */
        UnicodeCharMatrix(const std::string& utf8_text, size_t rows, size_t cols)
            : container_(rows, std::vector<char32_t>(cols, U' ')) {
            if (rows == 0 || cols == 0) {
                throw std::invalid_argument("Rows and cols must be greater than 0");
            }
            std::u32string u32_text = utf8_to_utf32(utf8_text);
            size_t line_idx = 0;
            size_t char_idx = 0;
            for (char32_t ch : u32_text) {
                if (line_idx >= rows) break;
                if (char_idx < cols) {
                    container_[line_idx][char_idx] = ch;
                    char_idx++;
                }
                else {
                    line_idx++;
                    char_idx = 0;
                    if (line_idx < rows) {
                        container_[line_idx][char_idx] = ch;
                        char_idx++;
                    }
                }
            }
        }

        /**
         Конструктор из UTF-8 строки с автоопределением размеров по \n
         Корректно работает с Non-ASCII символами
         */
        UnicodeCharMatrix(const std::string& utf8_lines) {
            if (utf8_lines.empty()) {
                throw std::invalid_argument("Input string is empty");
            }
            std::vector<std::u32string> split_lines;
            std::istringstream stream(utf8_lines);
            std::string line;
            size_t max_cols = 0;
            while (std::getline(stream, line)) {
                std::u32string u32_line = utf8_to_utf32(line);
                split_lines.push_back(u32_line);
                max_cols = std::max(max_cols, u32_line.size()); // Реальное количество символов
            }
            if (split_lines.empty()) {
                throw std::invalid_argument("No lines found in input string");
            }
            if (max_cols == 0) {
                throw std::invalid_argument("All lines are empty");
            }
            container_.resize(split_lines.size(), std::vector<char32_t>(max_cols, U' '));
            for (size_t i = 0; i < split_lines.size(); ++i) {
                for (size_t j = 0; j < split_lines[i].size(); ++j) {
                    container_[i][j] = split_lines[i][j];
                }
            }
        }

        /**
         Конструктор из списка UTF-8 строк
         Корректно работает с Non-ASCII символами
         */
        UnicodeCharMatrix(const std::initializer_list<std::string_view>& utf8_lines) {
            if (utf8_lines.size() == 0) {
                throw std::invalid_argument("Initializer list is empty");
            }
            std::vector<std::u32string> u32_lines;
            size_t max_cols = 0;
            for (const auto& line : utf8_lines) {
                std::u32string u32_line = utf8_to_utf32(std::string(line));
                u32_lines.push_back(u32_line);
                max_cols = std::max(max_cols, u32_line.size()); // Реальное количество символов!
            }
            if (max_cols == 0) {
                throw std::invalid_argument("All lines are empty");
            }
            container_.resize(u32_lines.size(), std::vector<char32_t>(max_cols, U' '));
            for (size_t i = 0; i < u32_lines.size(); ++i) {
                for (size_t j = 0; j < u32_lines[i].size(); ++j) {
                    container_[i][j] = u32_lines[i][j];
                }
            }
        }

        // Правило пяти
        UnicodeCharMatrix(const UnicodeCharMatrix&) = default;
        UnicodeCharMatrix(UnicodeCharMatrix&&) noexcept = default;
        UnicodeCharMatrix& operator=(const UnicodeCharMatrix&) = default;
        UnicodeCharMatrix& operator=(UnicodeCharMatrix&&) noexcept = default;
        ~UnicodeCharMatrix() = default;


        // ============================================================================
        // Геттеры размеров
        // ============================================================================

        size_t rows() const noexcept {
            return container_.size();
        }

        Result<size_t> cols() const noexcept {
            auto check = validate_not_empty();
            if (!check) {
                return Result<size_t>(check.error());
            }
            return Ok(container_[0].size());
        }

        bool empty() const noexcept {
            return container_.empty();
        }


        // ============================================================================
        // Работа с субматрицами
        // ============================================================================

        Result<UnicodeCharMatrix> submatrix(size_t start_row, size_t start_col,
            size_t end_row, size_t end_col) const {
            auto validation = validate_range(start_row, start_col, end_row, end_col);
            if (!validation) {
                return Result<UnicodeCharMatrix>(validation.error());
            }
            size_t sub_rows = end_row - start_row + 1;
            size_t sub_cols = end_col - start_col + 1;
            UnicodeCharMatrix result(U' ', sub_rows, sub_cols);
            for (size_t i = 0; i < sub_rows; ++i) {
                for (size_t j = 0; j < sub_cols; ++j) {
                    result.container_[i][j] = container_[start_row + i][start_col + j];
                }
            }
            return Ok(result);
        }

        Result<void> set_submatrix(size_t start_row, size_t start_col, const UnicodeCharMatrix& sub) {
            if (sub.empty()) {
                return Err(ErrorCode::INVALID_ARGUMENT, "Source submatrix is empty");
            }
            auto validation = validate_submatrix_fits(start_row, start_col,
                sub.rows(), sub.cols().value());
            if (!validation) {
                return validation;
            }
            for (size_t i = 0; i < sub.rows(); ++i) {
                for (size_t j = 0; j < sub.cols().value(); ++j) {
                    container_[start_row + i][start_col + j] = sub.container_[i][j];
                }
            }
            return Ok();
        }


        // ============================================================================
        // Работа со строками (UTF-8 интерфейс)
        // ============================================================================

        /**
         Установить строку из initializer_list Unicode символов
         */
        Result<void> set_row(size_t row, std::initializer_list<char32_t> chars) {
            auto row_check = validate_row(row);
            if (!row_check) return row_check;
            auto size_check = validate_size_matches_cols(chars.size());
            if (!size_check) return size_check;
            size_t i = 0;
            for (char32_t ch : chars) {
                container_[row][i++] = ch;
            }
            return Ok();
        }

        /**
         Установить строку из UTF-8 строки
         Корректно работает с Non-ASCII символами
         */
        Result<void> set_row(size_t row, const std::string& utf8_str) {
            auto row_check = validate_row(row);
            if (!row_check) return row_check;
            std::u32string u32_str = utf8_to_utf32(utf8_str);
            auto size_check = validate_size_matches_cols(u32_str.size());
            if (!size_check) return size_check;
            for (size_t i = 0; i < u32_str.size(); ++i) {
                container_[row][i] = u32_str[i];
            }
            return Ok();
        }

        /**
         Установить строку из string_view (UTF-8)
         */
        Result<void> set_row(size_t row, std::string_view utf8_str) {
            return set_row(row, std::string(utf8_str));
        }

        /**
         Установить строку из const char* (UTF-8)
         */
        Result<void> set_row(size_t row, const char* utf8_str) {
            return set_row(row, std::string(utf8_str));
        }

        /**
         Получить строку как UTF-8 строку
         Возвращает правильные Non-ASCII символы
         */
        Result<std::string> get_row(size_t row) const {
            auto validation = validate_row(row);
            if (!validation) {
                return Result<std::string>(validation.error());
            }
            std::u32string u32_row(container_[row].begin(), container_[row].end());
            return Ok(utf32_to_utf8(u32_row));
        }


        // ============================================================================
        // Работа с отдельными символами
        // ============================================================================

        /**
         Установить Unicode символ в позиции
         */
        Result<void> set_at(size_t row, size_t col, char32_t value) {
            auto validation = validate_position(row, col);
            if (!validation) 
                return validation;
            container_[row][col] = value;
            return Ok();
        }

        /**
         Получить Unicode символ из позиции
         */
        Result<char32_t> get_at(size_t row, size_t col) const {
            auto validation = validate_position(row, col);
            if (!validation) {
                return Result<char32_t>(validation.error());
            }
            return Ok(container_[row][col]);
        }

        /**
         Установить символ из UTF-8 строки (берется первый символ)
         Удобно для работы с однобуквенными UTF-8 литералами
         */
        Result<void> set_at_utf8(size_t row, size_t col, const std::string& utf8_char) {
            std::u32string u32_str = utf8_to_utf32(utf8_char);
            if (u32_str.empty()) {
                return Err(ErrorCode::INVALID_ARGUMENT, "Empty UTF-8 string");
            }
            return set_at(row, col, u32_str[0]);
        }

        /**
         Получить символ как UTF-8 строку
         */
        Result<std::string> get_at_utf8(size_t row, size_t col) const {
            auto ch_result = get_at(row, col);
            if (!ch_result) {
                return Result<std::string>(ch_result.error());
            }
            std::u32string u32_char(1, ch_result.value());
            return Ok(utf32_to_utf8(u32_char));
        }


        // ============================================================================
        // Дополнительные методы
        // ============================================================================

        /**
         Заполнить матрицу Unicode символом)
         */
        void fill_with(char32_t filler = U' ') {
            for (auto& row : container_) {
                std::fill(row.begin(), row.end(), filler);
            }
        }

        void clear() {
            fill_with();
        }

        /**
         Преобразовать в UTF-8 строку для вывода
         Корректно работает с Non-ASCII символами
         */
        std::string to_string() const {
            std::string result;
            for (const auto& row : container_) {
                std::u32string u32_row(row.begin(), row.end());
                result += utf32_to_utf8(u32_row);
                result += '\n';
            }
            return result;
        }

        /**
         Получить количество байт, которое займет матрица в UTF-8
         (для информации/отладки)
         */
        size_t utf8_byte_size() const {
            size_t total = 0;
            for (const auto& row : container_) {
                std::u32string u32_row(row.begin(), row.end());
                total += utf32_to_utf8(u32_row).size();
                total += 1; // \n
            }
            return total;
        }
    };


} // namespace dungeons::tui