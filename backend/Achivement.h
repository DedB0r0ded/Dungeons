#pragma once

#include "backend_base.h"


namespace dungeons::backend {

	class Achievement {
		std::string title_;
		std::string description_;
		bool unlocked_;
	public:
		// Конструктор
		Achievement() noexcept
			: title_(""), description_(""), unlocked_(false) {
		}
		Achievement(const std::string& title, const std::string& description) noexcept
			: title_(title), description_(description), unlocked_(false) {
		}


		// Правило пяти
		Achievement(const Achievement&) = default;
		Achievement(Achievement&&) noexcept = default;
		Achievement& operator=(const Achievement&) = default;
		Achievement& operator=(Achievement&&) noexcept = default;
		~Achievement() = default;


		// Геттеры
		const std::string& title() const noexcept { return title_; }
		const std::string& description() const noexcept { return description_; }
		bool is_unlocked() const noexcept { return unlocked_; }


		// Открыть достижение
		void unlock() noexcept { unlocked_ = true; }


		// Валидация
		::dungeons::Result<void> validate() const noexcept {
			if (title_.empty())
				return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Название достижения пустое");
			if (description_.empty())
				return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Описание достижения пустое");
			return ::dungeons::Ok();
		}


};


};
