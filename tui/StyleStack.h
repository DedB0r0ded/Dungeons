#pragma once


#include "tui_base.h"
#include "char_styling.h"


namespace dungeons::tui {


	class StyleStack {
		std::deque<CharStyle> container_;

		StyleStack(std::deque<CharStyle> container) : container_{ container } {}


	public:
		StyleStack() {
			container_ = std::deque<CharStyle>();
		}

		StyleStack(const StyleStack&) = delete;
		const StyleStack& operator=(const StyleStack&) = delete;
		StyleStack(StyleStack&&) = delete;
		const StyleStack& operator=(StyleStack&&) = delete;
		~StyleStack() = default;

		Result<CharStyle> peek_basic() const {
			if (container_.empty())
				return Err<CharStyle>(ErrorCode::OUT_OF_RANGE, "Style stack was empty.");
			return Ok(container_.back());
		}

		Result<CharStyle> peek() const {
			if (container_.empty())
				return Err<CharStyle>(ErrorCode::OUT_OF_RANGE, "Style stack was empty.");
			return Ok(container_.front());
		}

		void push(CharStyle value) {
			container_.push_front(value);
		}

		Result<void> pop() {
			if (container_.empty())
				return Err(ErrorCode::OUT_OF_RANGE, "Nothing to pop.");
			container_.pop_front();
			return Ok();
		}

		bool empty() const {
			return container_.empty();
		}

		size_t size() const {
			return container_.size();
		}

		void clear() {
			container_.clear();
		}
	};


}