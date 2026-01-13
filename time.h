// time.h

#pragma once

#include "result.h"
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

        // Приватный конструктор для внутреннего использования
        Time(uint16_t year, uint8_t month, uint8_t day, uint32_t milliseconds_from_midnight,
            int16_t timezone_offset_minutes, uint8_t leap_second, uint8_t is_dst,
            uint16_t microseconds, uint16_t reserved) noexcept
            : year_(year), month_(month), day_(day),
            milliseconds_from_midnight_(milliseconds_from_midnight),
            timezone_offset_minutes_(timezone_offset_minutes),
            leap_second_(leap_second), is_dst_(is_dst),
            microseconds_(microseconds), reserved_(reserved) {
        }

    public:
        // Конструкторы
        Time() noexcept : Time(1970, 1, 1, 0, 0, 0, 0, 0, 0) {}


        // Статический метод-фабрика вместо публичных конструкторов
        static Result<Time> create(uint16_t year, uint8_t month, uint8_t day,
            uint32_t milliseconds_from_midnight,
            int16_t timezone_offset_minutes = 0,
            uint8_t leap_second = 0,
            uint8_t is_dst = 0,
            uint16_t microseconds = 0,
            uint16_t reserved = 0) {
            Time t(year, month, day, milliseconds_from_midnight, timezone_offset_minutes,
                leap_second, is_dst, microseconds, reserved);

            auto validation = t.validate();
            if (!validation) {
                return Err<Time>(validation.error().code(), validation.error().message());
            }

            return Ok(t);
        }


        // Правило пяти
        Time(const Time&) = default;
        Time(Time&&) noexcept = default;
        Time& operator=(const Time&) = default;
        Time& operator=(Time&&) noexcept = default;
        ~Time() = default;


        // Геттеры
        uint16_t year() const noexcept { return year_; }
        uint8_t month() const noexcept { return month_; }
        uint8_t day() const noexcept { return day_; }
        uint8_t hours() const noexcept { return milliseconds_from_midnight_ / 3600000; }
        uint8_t minutes() const noexcept { return (milliseconds_from_midnight_ % 3600000) / 60000; }
        uint8_t seconds() const noexcept { return (milliseconds_from_midnight_ % 60000) / 1000; }
        uint16_t milliseconds() const noexcept { return milliseconds_from_midnight_ % 1000; }
        uint32_t milliseconds_from_midnight() const noexcept { return milliseconds_from_midnight_; }
        int16_t timezone_offset_minutes() const noexcept { return timezone_offset_minutes_; }
        bool leap_second() const noexcept { return leap_second_ != 0; }
        bool is_dst() const noexcept { return is_dst_ != 0; }
        uint16_t microseconds() const noexcept { return microseconds_; }


        // Сеттеры с валидацией
        Result<void> year(uint16_t value) noexcept {
            year_ = value;
            return validate();
        }

        Result<void> month(uint8_t value) noexcept {
            if (value < 1 || value > 12)
                return Err(ErrorCode::VALIDATION_FAILED,
                    "Month must be in range [1, 12]");
            month_ = value;
            return Ok();
        }

        Result<void> day(uint8_t value) noexcept {
            if (value < 1 || value > 31)
                return Err(ErrorCode::VALIDATION_FAILED,
                    "Day must be in range [1, 31]");
            day_ = value;
            return Ok();
        }

        Result<void> hours(uint8_t value) noexcept {
            if (value > 23) {
                return Err(ErrorCode::VALIDATION_FAILED,
                    "Hours must be in range [0, 23]");
            }
            uint32_t current_minutes = (milliseconds_from_midnight_ % 3600000) / 60000;
            uint32_t current_seconds = (milliseconds_from_midnight_ % 60000) / 1000;
            uint32_t current_millis = milliseconds_from_midnight_ % 1000;
            milliseconds_from_midnight_ = value * 3600000 +
                current_minutes * 60000 +
                current_seconds * 1000 +
                current_millis;
            return Ok();
        }

        Result<void> minutes(uint8_t value) noexcept {
            if (value > 59) {
                return Err(ErrorCode::VALIDATION_FAILED,
                    "Minutes must be in range [0, 59]");
            }
            uint32_t current_hours = milliseconds_from_midnight_ / 3600000;
            uint32_t current_seconds = (milliseconds_from_midnight_ % 60000) / 1000;
            uint32_t current_millis = milliseconds_from_midnight_ % 1000;
            milliseconds_from_midnight_ = current_hours * 3600000 +
                value * 60000 +
                current_seconds * 1000 +
                current_millis;
            return Ok();
        }

        Result<void> seconds(uint8_t value) noexcept {
            if (value > 59) {
                return Err(ErrorCode::VALIDATION_FAILED,
                    "Seconds must be in range [0, 59]");
            }
            uint32_t current_hours = milliseconds_from_midnight_ / 3600000;
            uint32_t current_minutes = (milliseconds_from_midnight_ % 3600000) / 60000;
            uint32_t current_millis = milliseconds_from_midnight_ % 1000;
            milliseconds_from_midnight_ = current_hours * 3600000 +
                current_minutes * 60000 +
                value * 1000 +
                current_millis;
            return Ok();
        }

        Result<void> milliseconds(uint16_t value) noexcept {
            if (value > 999) {
                return Err(ErrorCode::VALIDATION_FAILED,
                    "Milliseconds must be in range [0, 999]");
            }
            uint32_t current_hours = milliseconds_from_midnight_ / 3600000;
            uint32_t current_minutes = (milliseconds_from_midnight_ % 3600000) / 60000;
            uint32_t current_seconds = (milliseconds_from_midnight_ % 60000) / 1000;
            milliseconds_from_midnight_ = current_hours * 3600000 +
                current_minutes * 60000 +
                current_seconds * 1000 +
                value;
            return Ok();
        }

        Result<void> milliseconds_from_midnight(uint32_t value) noexcept {
            uint32_t max_millis = 86400000 + (leap_second_ ? 1000 : 0);
            if (value >= max_millis)
                return Err(ErrorCode::VALIDATION_FAILED,
                    "Milliseconds from midnight exceeds maximum for the day");
            milliseconds_from_midnight_ = value;
            return Ok();
        }

        Result<void> timezone_offset_minutes(int16_t value) noexcept {
            if (!valid_timezone_offset(value))
                return Err(ErrorCode::VALIDATION_FAILED,
                    "Timezone offset must be in range [-720, 840] minutes");
            timezone_offset_minutes_ = value;
            return Ok();
        }

        void leap_second(bool value) noexcept {
            leap_second_ = value ? 1 : 0;
        }

        void is_dst(bool value) noexcept {
            is_dst_ = value ? 1 : 0;
        }

        Result<void> set_microseconds(uint16_t value) noexcept {
            if (value >= 1000)
                return Err(ErrorCode::VALIDATION_FAILED,
                    "Microseconds must be in range [0, 999]");
            microseconds_ = value;
            return Ok();
        }


        // Валидация
        static bool valid_timezone_offset(int16_t timezone_offset_minutes) noexcept {
            return timezone_offset_minutes >= -720 && timezone_offset_minutes <= 840;
        }

        bool has_valid_timezone_offset() const noexcept {
            return valid_timezone_offset(timezone_offset_minutes_);
        }

        Result<void> validate() const noexcept {
            if (month_ < 1 || month_ > 12)
                return Err(ErrorCode::VALIDATION_FAILED,
                    "Invalid month: must be 1-12");
            if (day_ < 1 || day_ > 31)
                return Err(ErrorCode::VALIDATION_FAILED,
                    "Invalid day: must be 1-31");
            uint32_t max_millis = 86400000 + (leap_second_ ? 1000 : 0);
            if (milliseconds_from_midnight_ >= max_millis)
                return Err(ErrorCode::VALIDATION_FAILED,
                    "Invalid milliseconds");
            if (!has_valid_timezone_offset())
                return Err(ErrorCode::VALIDATION_FAILED,
                    "Invalid timezone offset");
            if (microseconds_ >= 1000)
                return Err(ErrorCode::VALIDATION_FAILED,
                    "Invalid microseconds: must be 0-999");
            return Ok();
        }


        // Устаревший метод для обратной совместимости с тестами
        std::string validate_legacy() const {
            auto result = validate();
            if (result.is_ok()) {
                return "";
            }
            return result.error().message();
        }


        // Перемещение в другой часовой пояс
        Result<void> move_to_timezone(int16_t target_timezone_offset_minutes) noexcept {
            if (!valid_timezone_offset(target_timezone_offset_minutes))
                return Err(ErrorCode::VALIDATION_FAILED,
                    "Target timezone offset is invalid");
            int32_t diff = target_timezone_offset_minutes - timezone_offset_minutes_;
            timezone_offset_minutes_ = target_timezone_offset_minutes;
            int64_t new_millis = static_cast<int64_t>(milliseconds_from_midnight_) + (diff * 60000);
            // Обработка перехода через границу суток
            if (new_millis < 0) {
                // TODO: уменьшить день
                new_millis += 86400000;
            }
            else if (new_millis >= 86400000) {
                // TODO: увеличить день
                new_millis -= 86400000;
            }
            milliseconds_from_midnight_ = static_cast<uint32_t>(new_millis);
            return Ok();
        }


        // Конвертация из/в стандартные типы
        static Result<Time> from_tm(const std::tm& tm_info, int16_t tz_offset = 0) noexcept {
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
            auto validation = result.validate();
            if (!validation)
                return Err<Time>(validation.error().code(), validation.error().message());
            return Ok(result);
        }

        static Result<Time> from_time_t(std::time_t t, int16_t tz_offset = 0) {
            auto tm_info = std::make_unique<tm>();
#ifdef _WIN32
            gmtime_s(tm_info.get(), &t);
#else
            tm_info = std::gmtime(&t);
#endif
            if (!tm_info)
                return Err<Time>(ErrorCode::INVALID_ARGUMENT, "Invalid time_t value");
            return from_tm(*tm_info, tz_offset);
        }

        std::tm to_tm() const noexcept {
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

        Result<std::time_t> to_time_t() const noexcept {
            auto validation = validate();
            if (!validation) {
                return Err<std::time_t>(validation.error().code(),
                    validation.error().message());
            }
            std::tm tm_info = to_tm();
            #ifdef _WIN32
                std::time_t result = _mkgmtime(&tm_info);
            #else
                std::time_t result = timegm(&tm_info);
            #endif
            if (result == -1) {
                return Err<std::time_t>(ErrorCode::INVALID_ARGUMENT,
                    "Failed to convert to time_t");
            }
            return Ok(result);
        }

        // Работа с system_clock
        static Result<Time> from_system_clock(const std::chrono::system_clock::time_point& tp,
            int16_t tz_offset = 0) {
            std::time_t t = std::chrono::system_clock::to_time_t(tp);
            auto duration_since_epoch = tp.time_since_epoch();
            auto total_micros = std::chrono::duration_cast<std::chrono::microseconds>(
                duration_since_epoch).count();
            auto micros_in_second = total_micros % 1000000;
            uint16_t milliseconds_part = static_cast<uint16_t>(micros_in_second / 1000);
            uint16_t microseconds_part = micros_in_second % 1000;
            auto result = from_time_t(t, tz_offset);
            if (!result) {
                return result;
            }
            Time time = result.value();
            auto millis_result = time.milliseconds(milliseconds_part);
            if (!millis_result) {
                return Err<Time>(millis_result.error().code(),
                    millis_result.error().message());
            }
            auto micros_result = time.set_microseconds(microseconds_part);
            if (!micros_result) {
                return Err<Time>(micros_result.error().code(),
                    micros_result.error().message());
            }
            return Ok(time);
        }

        Result<std::chrono::system_clock::time_point> to_system_clock() const {
            auto time_t_result = to_time_t();
            if (!time_t_result) {
                return Err<std::chrono::system_clock::time_point>(
                    time_t_result.error().code(),
                    time_t_result.error().message());
            }
            auto tp = std::chrono::system_clock::from_time_t(time_t_result.value());
            tp += std::chrono::milliseconds(milliseconds());
            tp += std::chrono::microseconds(microseconds());
            return Ok(tp);
        }

        // Конструктор из system_clock::time_point (для обратной совместимости)
        explicit Time(const std::chrono::system_clock::time_point& tp, int16_t tz_offset = 0) {
            auto result = from_system_clock(tp, tz_offset);
            if (result) {
                *this = result.value();
            }
            else {
                *this = Time(); // Fallback на время по умолчанию
            }
        }

        // Сериализация
        void to_bytes(uint8_t* buffer) const noexcept {
            std::memcpy(buffer, this, sizeof(Time));
        }

        static Result<Time> from_bytes(const uint8_t* buffer) {
            if (buffer == nullptr) {
                return Err<Time>(ErrorCode::INVALID_ARGUMENT,
                    "Buffer pointer is null");
            }
            Time t;
            std::memcpy(&t, buffer, sizeof(Time));
            auto validation = t.validate();
            if (!validation) {
                return Err<Time>(validation.error().code(),
                    validation.error().message());
            }
            return Ok(t);
        }

        // Операторы сравнения
        bool operator==(const Time& other) const noexcept {
            if (this == &other)
                return true;
            return std::memcmp(this, &other, sizeof(Time)) == 0;
        }

        bool operator!=(const Time& other) const noexcept {
            return !(*this == other);
        }

        bool operator<(const Time& other) const noexcept {
            if (year_ != other.year_) return year_ < other.year_;
            if (month_ != other.month_) return month_ < other.month_;
            if (day_ != other.day_) return day_ < other.day_;
            if (milliseconds_from_midnight_ != other.milliseconds_from_midnight_)
                return milliseconds_from_midnight_ < other.milliseconds_from_midnight_;
            return microseconds_ < other.microseconds_;
        }

        bool operator<=(const Time& other) const noexcept {
            return *this < other || *this == other;
        }

        bool operator>(const Time& other) const noexcept {
            return !(*this <= other);
        }

        bool operator>=(const Time& other) const noexcept {
            return !(*this < other);
        }


        // Строковое представление
        std::string to_string() const {
            char buffer[128];
            std::snprintf(buffer, sizeof(buffer),
                "%04u-%02u-%02u %02u:%02u:%02u.%03u.%03u%s (UTC%+03d:%02u)%s",
                year_, month_, day_, hours(), minutes(), seconds(),
                milliseconds(), microseconds(),
                leap_second() ? "+leap" : "",
                timezone_offset_minutes_ / 60,
                static_cast<unsigned>(std::abs(timezone_offset_minutes_ % 60)),
                is_dst() ? " DST" : "");
            return std::string(buffer);
        }

        std::string to_iso8601() const {
            char buffer[64];
            std::snprintf(buffer, sizeof(buffer),
                "%04u-%02u-%02uT%02u:%02u:%02u.%03u%+03d:%02u",
                year_, month_, day_, hours(), minutes(), seconds(),
                milliseconds(),
                timezone_offset_minutes_ / 60,
                static_cast<unsigned>(std::abs(timezone_offset_minutes_ % 60)));
            return std::string(buffer);
        }


        // Текущее время
        static Result<Time> now() {
            auto t_now = std::chrono::system_clock::now();
            return from_system_clock(t_now);
        }
    };
}