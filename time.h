#include <cstdint>
#include <cstring>
#include <string>
#include <ctime>
#include <chrono>


namespace dungeons {
	class Time {
	private:
		uint16_t year_;
		uint8_t month_;
		uint8_t day_;
		uint32_t milliseconds_from_midnight_;
		int16_t timezone_offset_minutes_;

		uint8_t leap_second_;
		uint8_t is_dst_;
		uint16_t microseconds_;
		uint16_t reserved_;

	public:

		// Статический метод для создания из system_clock::time_point
		static Time from_system_clock(const std::chrono::system_clock::time_point& tp, int16_t tz_offset = 0) {
			// Конвертируем time_point в time_t (получаем только целые секунды)
			std::time_t t = std::chrono::system_clock::to_time_t(tp);
			// Получаем полное количество микросекунд с начала эпохи
			auto duration_since_epoch = tp.time_since_epoch();
			auto total_micros = std::chrono::duration_cast<std::chrono::microseconds>(duration_since_epoch).count();
			// Извлекаем микросекунды внутри текущей секунды (0-999999)
			auto micros_in_second = total_micros % 1000000;
			// Разделяем на миллисекунды (0-999) и микросекунды (0-999)
			uint16_t milliseconds_part = micros_in_second / 1000;
			uint16_t microseconds_part = micros_in_second % 1000;
			// Создаем базовый Time из time_t (он даст нам дату и время с точностью до секунды)
			Time result = from_time_t(t, tz_offset);
			// Добавляем миллисекунды к milliseconds_from_midnight_
			result.milliseconds(milliseconds_part);
			// Устанавливаем микросекунды
			result.microseconds(microseconds_part);
			return result;
		}

		// Метод для конвертации в system_clock::time_point
		std::chrono::system_clock::time_point to_system_clock() const {
			// Конвертируем в time_t
			std::time_t t = to_time_t();
			// Создаем time_point из time_t
			auto tp = std::chrono::system_clock::from_time_t(t);
			// Добавляем миллисекунды
			tp += std::chrono::milliseconds(milliseconds());
			// Добавляем микросекунды
			tp += std::chrono::microseconds(microseconds());
			return tp;
		}


		// Конструктор из system_clock::time_point
		explicit Time(const std::chrono::system_clock::time_point& tp, int16_t tz_offset = 0)
			: Time(from_system_clock(tp, tz_offset)) {
		}

		Time(uint16_t year, uint8_t month, uint8_t day, uint32_t milliseconds_from_midnight,
			int16_t timezone_offset_minutes, uint8_t leap_second, uint8_t is_dst, uint16_t microseconds, uint16_t reserved) :
			year_{ year }, month_{ month }, day_{ day }, milliseconds_from_midnight_{ milliseconds_from_midnight }, timezone_offset_minutes_{ timezone_offset_minutes },
			leap_second_{ leap_second }, is_dst_{ is_dst }, microseconds_{ microseconds }, reserved_{ reserved } {}

		Time(uint16_t year, uint8_t month, uint8_t day, uint32_t milliseconds_from_midnight, int16_t timezone_offset_minutes) 
			: Time{ year, month, day, milliseconds_from_midnight, timezone_offset_minutes, 0, 0, 0, 0 } {}

		Time(uint16_t year, uint8_t month, uint8_t day, uint32_t milliseconds_from_midnight) : Time{ year, month, day, milliseconds_from_midnight, 0} {}

		Time() : Time{ 1970, 1, 1, 0 } {}


		uint8_t hours() const { return milliseconds_from_midnight_ / 3600000; }
		uint8_t minutes() const { return (milliseconds_from_midnight_ % 3600000) / 60000; }
		uint8_t seconds() const { return (milliseconds_from_midnight_ % 60000) / 1000; }
		uint16_t milliseconds() const { return milliseconds_from_midnight_ % 1000; }
		int16_t timezone_offset_minutes() const { return timezone_offset_minutes_; }
		bool leap_second() const { return leap_second_ != 0; }
		bool is_dst() const { return is_dst_ != 0; }
		uint16_t microseconds() const { return microseconds_; }


		void hours(uint8_t value) {
			if (value > 23)
				return;
			// Сохраняем текущие минуты, секунды и миллисекунды
			uint32_t current_minutes = (milliseconds_from_midnight_ % 3600000) / 60000;
			uint32_t current_seconds = (milliseconds_from_midnight_ % 60000) / 1000;
			uint32_t current_millis = milliseconds_from_midnight_ % 1000;
			// Пересчитываем milliseconds_from_midnight_
			milliseconds_from_midnight_ = value * 3600000 +
				current_minutes * 60000 +
				current_seconds * 1000 +
				current_millis;
		}

		void minutes(uint8_t value) {
			if (value > 59)
				return;
			// Сохраняем текущие часы, секунды и миллисекунды
			uint32_t current_hours = milliseconds_from_midnight_ / 3600000;
			uint32_t current_seconds = (milliseconds_from_midnight_ % 60000) / 1000;
			uint32_t current_millis = milliseconds_from_midnight_ % 1000;
			// Пересчитываем milliseconds_from_midnight_
			milliseconds_from_midnight_ = current_hours * 3600000 +
				value * 60000 +
				current_seconds * 1000 +
				current_millis;
		}

		void seconds(uint8_t value) {
			if (value > 59)
				return;
			// Сохраняем текущие часы, минуты и миллисекунды
			uint32_t current_hours = milliseconds_from_midnight_ / 3600000;
			uint32_t current_minutes = (milliseconds_from_midnight_ % 3600000) / 60000;
			uint32_t current_millis = milliseconds_from_midnight_ % 1000;
			// Пересчитываем milliseconds_from_midnight_
			milliseconds_from_midnight_ = current_hours * 3600000 +
				current_minutes * 60000 +
				value * 1000 +
				current_millis;
		}

		void milliseconds(uint16_t value) {
			if (value > 999)
				return;
			// Сохраняем текущие часы, минуты и секунды
			uint32_t current_hours = milliseconds_from_midnight_ / 3600000;
			uint32_t current_minutes = (milliseconds_from_midnight_ % 3600000) / 60000;
			uint32_t current_seconds = (milliseconds_from_midnight_ % 60000) / 1000;
			// Пересчитываем milliseconds_from_midnight_
			milliseconds_from_midnight_ = current_hours * 3600000 +
				current_minutes * 60000 +
				current_seconds * 1000 +
				value;
		}

		void milliseconds_from_midnight(uint32_t value) {
			this->milliseconds_from_midnight_ = value;
		}

		void timezone_offset_minutes(int16_t value) {
			this->timezone_offset_minutes_ = value;
		}

		void leap_second(bool value) {
			this->leap_second_ = value ? 1 : 0;
		}

		void is_dst(bool value) {
			this->is_dst_ = value ? 1 : 0;
		}

		void microseconds(uint16_t value) {
			if (value >= 1000)
				return;
			this->microseconds_ = value;
		}

		void move_to_timezone(uint16_t target_timezone_offset_minutes) {
			if (!valid_timezone_offset(target_timezone_offset_minutes))
				return; // TODO: Exception/Result?
			uint16_t diff = timezone_offset_minutes_ > target_timezone_offset_minutes ? timezone_offset_minutes_ - target_timezone_offset_minutes : target_timezone_offset_minutes - timezone_offset_minutes_;
			timezone_offset_minutes_ = target_timezone_offset_minutes;
			milliseconds_from_midnight_ += diff * 60'000;
		}


		static bool valid_timezone_offset(uint16_t timezone_offset_minutes) {
			return timezone_offset_minutes > -720 || timezone_offset_minutes < 840;
		}

		bool has_valid_timezone_offset() const {
			return valid_timezone_offset(this->timezone_offset_minutes());
		}

		// TODO: return Exception/Result?
		// TODO: separate validations
		std::string validate() const {
			if (month_ < 1 || month_ > 12) {
				return "Invalid month: must be 1-12";
			}
			if (day_ < 1 || day_ > 31) {
				return "Invalid day: must be 1-31";
			}
			// 86400000 мс в сутках, + 1000 для високосной секунды
			if (milliseconds_from_midnight_ >= 86400000 + (leap_second_ ? 1000 : 0)) {
				return "Invalid milliseconds";
			}
			if (!has_valid_timezone_offset()) {
				return "Invalid timezone offset";
			}
			if (microseconds_ >= 1000) {
				return "Invalid microseconds: must be 0-999";
			}
			return "";
		}


		// Конвертация в стандартные типы
		static Time from_tm(const std::tm& tm_info, int16_t tz_offset = 0) {
			Time result;
			result.year_ = tm_info.tm_year + 1900;
			result.month_ = tm_info.tm_mon + 1;
			result.day_ = tm_info.tm_mday;
			result.milliseconds_from_midnight_ =
				tm_info.tm_hour * 3600000 +
				tm_info.tm_min * 60000 +
				tm_info.tm_sec * 1000;
			result.timezone_offset_minutes_ = tz_offset;
			result.leap_second_ = 0;
			result.is_dst_ = tm_info.tm_isdst > 0 ? 1 : 0;
			result.microseconds_ = 0;
			result.reserved_ = 0;
			return result;
		}

		static Time from_time_t(std::time_t t, int16_t tz_offset = 0) {
			std::tm* tm_info = std::gmtime(&t);
			if (!tm_info) {
				throw std::invalid_argument("Invalid time_t value");
			}
			return from_tm(*tm_info, tz_offset);
		}
		
		std::tm to_tm() const{
			std::tm tm_info = {};
			tm_info.tm_year = year_ - 1900;
			tm_info.tm_mon = month_ - 1;
			tm_info.tm_mday = day_;
			tm_info.tm_hour = hours();
			tm_info.tm_min = minutes();
			tm_info.tm_sec = seconds();
			tm_info.tm_isdst = is_dst_;
			return tm_info;
		}

		std::time_t to_time_t() const{
			std::tm tm_info = to_tm();
			#ifdef _WIN32
				return _mkgmtime(&tm_info);
			#else
				return timegm(&tm_info);
			#endif
		}


		// Сериализация
		void to_bytes(uint8_t* buffer) const {
			std::memcpy(buffer, this, sizeof(Time));
		}

		static Time from_bytes(const uint8_t* buffer) {
			Time t;
			std::memcpy(&t, buffer, sizeof(Time));
			t.validate();
			return t;
		}


		// Операторы сравнения
		bool operator==(const Time& other) const {
			return std::memcmp(this, &other, sizeof(Time)) == 0;
		}

		bool operator!=(const Time& other) const {
			return !(*this == other);
		}

		bool operator<(const Time& other) const {
			// Сравниваем как UNIX timestamp
			if (year_ != other.year_) return year_ < other.year_;
			if (month_ != other.month_) return month_ < other.month_;
			if (day_ != other.day_) return day_ < other.day_;
			if (milliseconds_from_midnight_ != other.milliseconds_from_midnight_)
				return milliseconds_from_midnight_ < other.milliseconds_from_midnight_;
			return microseconds_ < other.microseconds_;
		}


		// Строковое представление
		std::string to_string() const {
			char buffer[128];
			snprintf(buffer, sizeof(buffer),
				"%04u-%02u-%02u %02u:%02u:%02u.%03u.%03u%s (UTC%+03d:%02u)%s",
				year_, month_, day_, hours(), minutes(), seconds(), milliseconds(), microseconds(),
				leap_second() ? "+leap" : "",
				timezone_offset_minutes_ / 60, abs(timezone_offset_minutes_ % 60),
				is_dst() ? " DST" : "");
			return std::string(buffer);
		}

		// ISO 8601 формат
		std::string to_iso8601() const {
			char buffer[64];
			snprintf(buffer, sizeof(buffer),
				"%04u-%02u-%02uT%02u:%02u:%02u.%03u%+03d:%02u",
				year_, month_, day_, hours(), minutes(), seconds(),
				milliseconds(),
				timezone_offset_minutes_ / 60,
				abs(timezone_offset_minutes_ % 60));
			return std::string(buffer);
		}

		static Time now() {
			auto t_now = std::chrono::system_clock::now();
			return Time(t_now);
		}
	};
}