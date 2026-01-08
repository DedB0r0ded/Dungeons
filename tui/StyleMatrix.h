#pragma once


#include "tui_base.h"
#include "char_styling.h"


namespace dungeons::tui {


	class StyleMatrix {
		std::vector<std::vector<CharStyle>> container_;
        CharStyle default_style_;

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
        // Конструкторы
        // ============================================================================

        StyleMatrix(CharStyle defaultStyle, size_t rows, size_t cols) 
            : container_(rows, std::vector<CharStyle>(cols, defaultStyle)), default_style_(defaultStyle) {
            if (rows == 0 || cols == 0) {
                throw std::invalid_argument("Rows and cols must be greater than 0");
            }
        }

        StyleMatrix(const StyleMatrix& other) : container_(other.container_), default_style_(other.default_style_) { }

        StyleMatrix(StyleMatrix&& other) noexcept : container_(std::move(other.container_)), default_style_(std::move(other.default_style_)) { }

        StyleMatrix& operator=(const StyleMatrix& other) {
            if (this != &other) {
                container_ = other.container_;
                default_style_ = other.default_style_;
            }
            return *this;
        }

        StyleMatrix& operator=(StyleMatrix&& other) noexcept {
            if (this != &other) {
                container_ = std::move(other.container_);
                default_style_ = std::move(other.default_style_);
            }
            return *this;
        }

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

        CharStyle default_style() const {
            return default_style_;
        }

        // ============================================================================
        // Работа с субматрицами
        // ============================================================================

        Result<StyleMatrix> submatrix(size_t start_row, size_t start_col, size_t end_row, size_t end_col) {
            auto validation = validate_range(start_row, start_col, end_row, end_col);
            if (!validation)
                return Result<StyleMatrix>(validation.error());
            size_t sub_rows = end_row - start_row + 1;
            size_t sub_cols = end_col - start_col + 1;
            StyleMatrix result(default_style_, sub_rows, sub_cols);
            for (size_t i = 0; i < sub_rows; ++i) {
                for (size_t j = 0; j < sub_cols; ++j) {
                    result.container_[i][j] = container_[start_row + i][start_col + j];
                }
            }
            return Ok(result);
        }

        Result<void> set_submatrix(size_t start_row, size_t start_col, StyleMatrix sub) {
            if (sub.empty())
                return Err(ErrorCode::INVALID_ARGUMENT, "Source submatrix is empty");
            auto validation = validate_submatrix_fits(start_row, start_col, sub.rows(), sub.cols().value());
            if (!validation)
                return validation;
            for (size_t i = 0; i < sub.rows(); ++i) {
                for (size_t j = 0; j < sub.cols().value(); ++j) {
                    container_[start_row + i][start_col + j] = sub.container_[i][j];
                }
            }
            return Ok();
        }
        
        // ============================================================================
        // Работа с областями и отдельными элементами
        // ============================================================================

        Result<void> set_area(size_t start_row, size_t start_col,
            size_t end_row, size_t end_col, CharStyle style) {
            auto validation = validate_range(start_row, start_col, end_row, end_col);
            if (!validation)
                return validation;
            auto style_validation = style.validate();
            if (!style_validation)
                return style_validation;
            for (size_t i = start_row; i <= end_row; ++i) {
                for (size_t j = start_col; j <= end_col; ++j) {
                    container_[i][j] = style;
                }
            }
            return Ok();
        }

        Result<void> set_row(size_t row, CharStyle style) {
            auto row_check = validate_row(row);
            if (!row_check)
                return row_check;
            auto style_validation = style.validate();
            if (!style_validation)
                return style_validation;
            for (size_t j = 0; j < container_[row].size(); ++j) {
                container_[row][j] = style;
            }
            return Ok();
        }

        Result<void> set_at(size_t row, size_t col, CharStyle style) {
            auto validation = validate_position(row, col);
            if (!validation)
                return validation;
            auto style_validation = style.validate();
            if (!style_validation)
                return style_validation;
            container_[row][col] = style;
            return Ok();
        }

        Result<CharStyle> at(size_t row, size_t col) const  {
            auto validation = validate_position(row, col);
            if (!validation)
                return Result<CharStyle>(validation.error());
            return Ok(container_[row][col]);
        }


        Result<void> set_default_style(const CharStyle& value) {
            auto validation = value.validate();
            if (!validation) {
                return validation;
            }
            default_style_ = value;
            return Ok();
        }


        void fill_with(CharStyle filler) {
            for (auto& row : container_) {
                std::fill(row.begin(), row.end(), filler);
            }
        }

        void clear() {
            fill_with(default_style_);
        }
	};


}