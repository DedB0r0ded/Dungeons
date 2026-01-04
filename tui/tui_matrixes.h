#pragma once


#include "tui_base.h"


namespace dungeons::tui {


	class CharMatrix {
		std::vector<std::vector<char>> rows_;
		const char fill_;

	public:
		CharMatrix(size_t rows, size_t cols, char fill) : fill_{ fill } {
			rows_ = std::vector<std::vector<char>>(rows);
			for (auto r : rows_) {
				r = std::vector<char>(cols);
			}
		}

		std::vector<char>& operator[](size_t id) {
			return rows_[id];
		}

		char& at(size_t row, size_t col) {
			return rows_.at(row).at(col);
		}

		size_t rows() {
			return rows_.size();
		}
	};

	class StyleMatrix {

	};


}