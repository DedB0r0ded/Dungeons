#pragma once


#include "tui_base.h"


namespace dungeons::tui {


    class RawCharMatrix {
        std::vector<std::vector<char>> container_;

        // ============================================================================
        // Вспомогательные методы валидации
        // ============================================================================

        // Проверка, что матрица не пустая
        Result<void> validate_not_empty() const noexcept {
            if (container_.empty()) {
                return Err(ErrorCode::EMPTY_CONTAINER, "Matrix is empty");
            }
            return Ok();
        }

        // Проверка валидности индекса строки
        Result<void> validate_row(size_t row) const noexcept {
            auto check = validate_not_empty();
            if (!check) return check;

            if (row >= container_.size()) {
                return Err(ErrorCode::OUT_OF_RANGE,
                    "Row " + std::to_string(row) + " is out of range [0, " +
                    std::to_string(container_.size()) + ")");
            }
            return Ok();
        }

        // Проверка валидности индекса столбца
        Result<void> validate_col(size_t col) const noexcept {
            auto check = validate_not_empty();
            if (!check) return check;
            if (col >= container_[0].size()) {
                return Err(ErrorCode::OUT_OF_RANGE,
                    "Column " + std::to_string(col) + " is out of range [0, " +
                    std::to_string(container_[0].size()) + ")");
            }
            return Ok();
        }

        // Проверка валидности позиции (row, col)
        Result<void> validate_position(size_t row, size_t col) const noexcept {
            auto row_check = validate_row(row);
            if (!row_check) return row_check;
            auto col_check = validate_col(col);
            if (!col_check) return col_check;
            return Ok();
        }

        // Проверка валидности диапазона строк
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

        // Проверка валидности диапазона столбцов
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

        // Проверка валидности диапазона для submatrix
        Result<void> validate_range(size_t start_row, size_t start_col,
            size_t end_row, size_t end_col) const noexcept {
            auto row_check = validate_row_range(start_row, end_row);
            if (!row_check) return row_check;
            auto col_check = validate_col_range(start_col, end_col);
            if (!col_check) return col_check;
            return Ok();
        }

        // Проверка, что размер соответствует количеству столбцов
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

        // Проверка, что субматрица помещается начиная с позиции
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
        // Конструкторы
        // ============================================================================

        // Конструктор с заполнением символом
        RawCharMatrix(const char filler, const size_t rows, const size_t cols)
            : container_(rows, std::vector<char>(cols, filler)) {
            if (rows == 0 || cols == 0) {
                throw std::invalid_argument("Rows and cols must be greater than 0");
            }
        }

        // Конструктор из строки с явными размерами
        RawCharMatrix(const std::string& lines, const size_t rows, const size_t cols)
            : container_(rows, std::vector<char>(cols, ' ')) {
            if (rows == 0 || cols == 0) {
                throw std::invalid_argument("Rows and cols must be greater than 0");
            }
            size_t line_idx = 0;
            size_t char_idx = 0;
            for (char ch : lines) {
                if (line_idx >= rows) break;
                if (char_idx < cols) {
                    container_[line_idx][char_idx] = ch;
                    char_idx++;
                } else {
                    line_idx++;
                    char_idx = 0;
                }
            }
        }

        // Конструктор из строки с автоопределением размеров по \n
        RawCharMatrix(const std::string& lines) {
            if (lines.empty()) {
                throw std::invalid_argument("Input string is empty");
            }
            std::vector<std::string> split_lines;
            std::istringstream stream(lines);
            std::string line;
            size_t max_cols = 0;
            while (std::getline(stream, line)) {
                split_lines.push_back(line);
                max_cols = std::max(max_cols, line.size());
            }
            if (split_lines.empty()) {
                throw std::invalid_argument("No lines found in input string");
            }
            if (max_cols == 0) {
                throw std::invalid_argument("All lines are empty");
            }
            container_.resize(split_lines.size(), std::vector<char>(max_cols, ' '));
            for (size_t i = 0; i < split_lines.size(); ++i) {
                for (size_t j = 0; j < split_lines[i].size(); ++j) {
                    container_[i][j] = split_lines[i][j];
                }
            }
        }

        // Конструктор из списка инициализации string_view
        RawCharMatrix(const std::initializer_list<std::string_view>& lines) {
            if (lines.size() == 0) {
                throw std::invalid_argument("Initializer list is empty");
            }
            size_t max_cols = 0;
            for (const auto& line : lines) {
                max_cols = std::max(max_cols, line.size());
            }
            if (max_cols == 0) {
                throw std::invalid_argument("All lines are empty");
            }
            container_.resize(lines.size(), std::vector<char>(max_cols, ' '));
            size_t i = 0;
            for (const auto& line : lines) {
                for (size_t j = 0; j < line.size(); ++j) {
                    container_[i][j] = line[j];
                }
                ++i;
            }
        }

        // Правило пяти
        RawCharMatrix(const RawCharMatrix&) = default;
        RawCharMatrix(RawCharMatrix&&) noexcept = default;
        RawCharMatrix& operator=(const RawCharMatrix&) = default;
        RawCharMatrix& operator=(RawCharMatrix&&) noexcept = default;
        ~RawCharMatrix() = default;


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


        // ============================================================================
        // Работа с субматрицами
        // ============================================================================

        Result<RawCharMatrix> submatrix(size_t start_row, size_t start_col,
            size_t end_row, size_t end_col) const {
            auto validation = validate_range(start_row, start_col, end_row, end_col);
            if (!validation) {
                return Result<RawCharMatrix>(validation.error());
            }
            size_t sub_rows = end_row - start_row + 1;
            size_t sub_cols = end_col - start_col + 1;
            RawCharMatrix result(' ', sub_rows, sub_cols);
            for (size_t i = 0; i < sub_rows; ++i) {
                for (size_t j = 0; j < sub_cols; ++j) {
                    result.container_[i][j] = container_[start_row + i][start_col + j];
                }
            }
            return Ok(result);
        }

        Result<void> set_submatrix(size_t start_row, size_t start_col, const RawCharMatrix& sub) {
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
        // Работа со строками
        // ============================================================================

        Result<void> set_row(size_t row, std::initializer_list<char> chars) {
            auto row_check = validate_row(row);
            if (!row_check)
                return row_check;
            auto size_check = validate_size_matches_cols(chars.size());
            if (!size_check)
                return size_check;
            size_t i = 0;
            for (char ch : chars) {
                container_[row][i++] = ch;
            }
            return Ok();
        }

        Result<void> set_row(size_t row, std::string_view str) {
            auto row_check = validate_row(row);
            if (!row_check)
                return row_check;
            auto size_check = validate_size_matches_cols(str.size());
            if (!size_check)
                return size_check;
            for (size_t i = 0; i < str.size(); ++i) {
                container_[row][i] = str[i];
            }
            return Ok();
        }

        Result<void> set_row(size_t row, const std::string& str) {
            return set_row(row, std::string_view(str));
        }

        Result<void> set_row(size_t row, const char* str) {
            return set_row(row, std::string_view(str));
        }

        Result<std::string> get_row(size_t row) const {
            auto validation = validate_row(row);
            if (!validation) {
                return Err<std::string>(validation.error().code(), validation.error().message());
            }
            return Ok(std::string(container_[row].begin(), container_[row].end()));
        }


        // ============================================================================
        // Работа с отдельными символами
        // ============================================================================

        Result<void> set_at(size_t row, size_t col, char value) {
            auto validation = validate_position(row, col);
            if (!validation) return validation;
            container_[row][col] = value;
            return Ok();
        }

        Result<char> get_at(size_t row, size_t col) const {
            auto validation = validate_position(row, col);
            if (!validation) {
                return Err<char>(validation.error().code(), validation.error().message());
            }
            return Ok(container_[row][col]);
        }


        // ============================================================================
        // Дополнительные методы
        // ============================================================================

        void clear(char filler = ' ') {
            for (auto& row : container_) {
                std::fill(row.begin(), row.end(), filler);
            }
        }

        std::string to_string() const {
            std::string result;
            for (const auto& row : container_) {
                result.append(row.begin(), row.end());
                result += '\n';
            }
            return result;
        }

        bool empty() const noexcept {
            return container_.empty();
        }
    };


}