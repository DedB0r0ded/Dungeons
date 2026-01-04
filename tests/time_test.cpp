#include <gtest/gtest.h>
#include "../time.h"

using namespace dungeons;

class TimeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Потенциально добавить общую инициализацию
    }
};

// ========== Мета-тесты ==========
TEST_F(TimeTest, TypeSize) {
    EXPECT_EQ(sizeof(Time), 16);
}

// ========== Тесты конструкторов ==========

TEST_F(TimeTest, DefaultConstructor) {
    Time t;
    EXPECT_EQ(t.hours(), 0);
    EXPECT_EQ(t.minutes(), 0);
    EXPECT_EQ(t.seconds(), 0);
    EXPECT_EQ(t.milliseconds(), 0);
    EXPECT_FALSE(t.leap_second());
    EXPECT_FALSE(t.is_dst());
    EXPECT_EQ(t.microseconds(), 0);
}

TEST_F(TimeTest, ConstructorWithBasicParams) {
    Time t(2024, 12, 25, 45296789); // 12:34:56.789
    EXPECT_EQ(t.hours(), 12);
    EXPECT_EQ(t.minutes(), 34);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 789);
}

TEST_F(TimeTest, ConstructorWithTimezoneOffset) {
    Time t(2024, 12, 25, 45296789, 180); // 12:34:56.789 UTC+03
    EXPECT_EQ(t.hours(), 12);
    EXPECT_EQ(t.minutes(), 34);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 789);
    EXPECT_EQ(t.timezone_offset_minutes(), 180);
}

TEST_F(TimeTest, ConstructorWithAllParams) {
    Time t(2024, 12, 25, 45296789, 120, 1, 1, 500, 0);
    EXPECT_EQ(t.hours(), 12);
    EXPECT_EQ(t.minutes(), 34);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 789);
    EXPECT_TRUE(t.leap_second());
    EXPECT_TRUE(t.is_dst());
    EXPECT_EQ(t.microseconds(), 500);
}

// ========== Тесты геттеров времени ==========

TEST_F(TimeTest, TimeComponents_Midnight) {
    Time t(2024, 1, 1, 0);
    EXPECT_EQ(t.hours(), 0);
    EXPECT_EQ(t.minutes(), 0);
    EXPECT_EQ(t.seconds(), 0);
    EXPECT_EQ(t.milliseconds(), 0);
}

TEST_F(TimeTest, TimeComponents_Noon) {
    Time t(2024, 1, 1, 43200000); // 12:00:00.000
    EXPECT_EQ(t.hours(), 12);
    EXPECT_EQ(t.minutes(), 0);
    EXPECT_EQ(t.seconds(), 0);
    EXPECT_EQ(t.milliseconds(), 0);
}

TEST_F(TimeTest, TimeComponents_AlmostMidnight) {
    Time t(2024, 1, 1, 86399999); // 23:59:59.999
    EXPECT_EQ(t.hours(), 23);
    EXPECT_EQ(t.minutes(), 59);
    EXPECT_EQ(t.seconds(), 59);
    EXPECT_EQ(t.milliseconds(), 999);
}

TEST_F(TimeTest, TimeComponents_ArbitraryTime) {
    Time t(2024, 6, 15, 52875432); // 14:41:15.432
    EXPECT_EQ(t.hours(), 14);
    EXPECT_EQ(t.minutes(), 41);
    EXPECT_EQ(t.seconds(), 15);
    EXPECT_EQ(t.milliseconds(), 432);
}

// ========== Тесты сеттеров ==========

TEST_F(TimeTest, SetMillisecondsFromMidnight) {
    Time t;
    t.milliseconds_from_midnight(45296789);
    EXPECT_EQ(t.hours(), 12);
    EXPECT_EQ(t.minutes(), 34);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 789);
}

TEST_F(TimeTest, SetLeapSecond) {
    Time t;
    t.leap_second(true);
    EXPECT_TRUE(t.leap_second());
    t.leap_second(false);
    EXPECT_FALSE(t.leap_second());
}

TEST_F(TimeTest, SetIsDst) {
    Time t;
    t.is_dst(true);
    EXPECT_TRUE(t.is_dst());
    t.is_dst(false);
    EXPECT_FALSE(t.is_dst());
}

TEST_F(TimeTest, SetMicroseconds_Valid) {
    Time t;
    t.microseconds(500);
    EXPECT_EQ(t.microseconds(), 500);
    t.microseconds(999);
    EXPECT_EQ(t.microseconds(), 999);
    t.microseconds(0);
    EXPECT_EQ(t.microseconds(), 0);
}

TEST_F(TimeTest, SetMicroseconds_Invalid) {
    Time t;
    t.microseconds(500);
    t.microseconds(1000); // Недопустимое значение
    EXPECT_EQ(t.microseconds(), 500); // Должно остаться прежнее значение

    t.microseconds(1500);
    EXPECT_EQ(t.microseconds(), 500);
}

// ========== Дополнительные тесты для сеттеров компонентов времени ==========

TEST_F(TimeTest, SetHours_Valid) {
    Time t(2024, 6, 15, 45296789); // 12:34:56.789
    t.hours(18);
    EXPECT_EQ(t.hours(), 18);
    EXPECT_EQ(t.minutes(), 34);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 789);
}

TEST_F(TimeTest, SetHours_Invalid) {
    Time t(2024, 6, 15, 45296789); // 12:34:56.789
    t.hours(24); // Недопустимое значение
    EXPECT_EQ(t.hours(), 12); // Должно остаться прежнее значение
    EXPECT_EQ(t.minutes(), 34);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 789);
}

TEST_F(TimeTest, SetHours_Boundary) {
    Time t(2024, 6, 15, 45296789);
    t.hours(0);
    EXPECT_EQ(t.hours(), 0);
    t.hours(23);
    EXPECT_EQ(t.hours(), 23);
}

TEST_F(TimeTest, SetMinutes_Valid) {
    Time t(2024, 6, 15, 45296789); // 12:34:56.789
    t.minutes(45);
    EXPECT_EQ(t.hours(), 12);
    EXPECT_EQ(t.minutes(), 45);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 789);
}

TEST_F(TimeTest, SetMinutes_Invalid) {
    Time t(2024, 6, 15, 45296789); // 12:34:56.789
    t.minutes(60); // Недопустимое значение
    EXPECT_EQ(t.hours(), 12);
    EXPECT_EQ(t.minutes(), 34); // Должно остаться прежнее значение
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 789);
}

TEST_F(TimeTest, SetMinutes_Boundary) {
    Time t(2024, 6, 15, 45296789);
    t.minutes(0);
    EXPECT_EQ(t.minutes(), 0);
    t.minutes(59);
    EXPECT_EQ(t.minutes(), 59);
}

TEST_F(TimeTest, SetSeconds_Valid) {
    Time t(2024, 6, 15, 45296789); // 12:34:56.789
    t.seconds(30);
    EXPECT_EQ(t.hours(), 12);
    EXPECT_EQ(t.minutes(), 34);
    EXPECT_EQ(t.seconds(), 30);
    EXPECT_EQ(t.milliseconds(), 789);
}

TEST_F(TimeTest, SetSeconds_Invalid) {
    Time t(2024, 6, 15, 45296789); // 12:34:56.789
    t.seconds(60); // Недопустимое значение
    EXPECT_EQ(t.hours(), 12);
    EXPECT_EQ(t.minutes(), 34);
    EXPECT_EQ(t.seconds(), 56); // Должно остаться прежнее значение
    EXPECT_EQ(t.milliseconds(), 789);
}

TEST_F(TimeTest, SetSeconds_Boundary) {
    Time t(2024, 6, 15, 45296789);
    t.seconds(0);
    EXPECT_EQ(t.seconds(), 0);
    t.seconds(59);
    EXPECT_EQ(t.seconds(), 59);
}

TEST_F(TimeTest, SetMilliseconds_Valid) {
    Time t(2024, 6, 15, 45296789); // 12:34:56.789
    t.milliseconds(123);
    EXPECT_EQ(t.hours(), 12);
    EXPECT_EQ(t.minutes(), 34);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 123);
}

TEST_F(TimeTest, SetMilliseconds_Invalid) {
    Time t(2024, 6, 15, 45296789); // 12:34:56.789
    t.milliseconds(1000); // Недопустимое значение
    EXPECT_EQ(t.hours(), 12);
    EXPECT_EQ(t.minutes(), 34);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 789); // Должно остаться прежнее значение
}

TEST_F(TimeTest, SetMilliseconds_Boundary) {
    Time t(2024, 6, 15, 45296789);
    t.milliseconds(0);
    EXPECT_EQ(t.milliseconds(), 0);
    t.milliseconds(999);
    EXPECT_EQ(t.milliseconds(), 999);
}

TEST_F(TimeTest, SetMultipleComponents) {
    Time t(2024, 6, 15, 0);
    t.hours(15);
    t.minutes(30);
    t.seconds(45);
    t.milliseconds(500);

    EXPECT_EQ(t.hours(), 15);
    EXPECT_EQ(t.minutes(), 30);
    EXPECT_EQ(t.seconds(), 45);
    EXPECT_EQ(t.milliseconds(), 500);
}

TEST_F(TimeTest, SetComponents_PreserveOthers) {
    Time t(2024, 6, 15, 45296789); // 12:34:56.789

    // Изменяем только часы
    t.hours(20);
    EXPECT_EQ(t.hours(), 20);
    EXPECT_EQ(t.minutes(), 34);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 789);

    // Изменяем только минуты
    t.minutes(15);
    EXPECT_EQ(t.hours(), 20);
    EXPECT_EQ(t.minutes(), 15);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 789);
}

// ========== Тесты валидации ==========

TEST_F(TimeTest, Validate_ValidTime) {
    Time t(2024, 6, 15, 45000000, 180, 0, 0, 500, 0);
    EXPECT_EQ(t.validate(), "");
}

TEST_F(TimeTest, Validate_InvalidMonth_TooLow) {
    Time t(2024, 0, 15, 0);
    EXPECT_EQ(t.validate(), "Invalid month: must be 1-12");
}

TEST_F(TimeTest, Validate_InvalidMonth_TooHigh) {
    Time t(2024, 13, 15, 0);
    EXPECT_EQ(t.validate(), "Invalid month: must be 1-12");
}

TEST_F(TimeTest, Validate_InvalidDay_TooLow) {
    Time t(2024, 6, 0, 0);
    EXPECT_EQ(t.validate(), "Invalid day: must be 1-31");
}

TEST_F(TimeTest, Validate_InvalidDay_TooHigh) {
    Time t(2024, 6, 32, 0);
    EXPECT_EQ(t.validate(), "Invalid day: must be 1-31");
}

TEST_F(TimeTest, Validate_InvalidMilliseconds_NoLeapSecond) {
    Time t(2024, 6, 15, 86400000); // Ровно 24 часа
    EXPECT_EQ(t.validate(), "Invalid milliseconds");
}

TEST_F(TimeTest, Validate_ValidMilliseconds_WithLeapSecond) {
    Time t(2024, 6, 15, 86400500, 0, 1, 0, 0, 0); // С високосной секундой
    EXPECT_EQ(t.validate(), "");
}

TEST_F(TimeTest, Validate_InvalidMilliseconds_WithLeapSecond) {
    Time t(2024, 6, 15, 86401000, 0, 1, 0, 0, 0); // Превышает даже с високосной
    EXPECT_EQ(t.validate(), "Invalid milliseconds");
}

TEST_F(TimeTest, Validate_InvalidTimezone_TooLow) {
    Time t(2024, 6, 15, 0);
    t.timezone_offset_minutes(-721);
    EXPECT_EQ(t.validate(), "Invalid timezone offset");
}

TEST_F(TimeTest, Validate_InvalidTimezone_TooHigh) {
    Time t(2024, 6, 15, 0);
    t.timezone_offset_minutes(841);
    EXPECT_EQ(t.validate(), "Invalid timezone offset");
}

TEST_F(TimeTest, Validate_ValidTimezone_Boundaries) {
    Time t1(2024, 6, 15, 0);
    t1.timezone_offset_minutes(-720);
    EXPECT_EQ(t1.validate(), "");

    Time t2(2024, 6, 15, 0);
    t2.timezone_offset_minutes(840);
    EXPECT_EQ(t2.validate(), "");
}

TEST_F(TimeTest, Validate_InvalidMicroseconds) {
    Time t(2024, 6, 15, 0, 0, 0, 0, 1000, 0);
    EXPECT_EQ(t.validate(), "Invalid microseconds: must be 0-999");
}

// ========== Тесты from_tm и to_tm ==========

TEST_F(TimeTest, FromTm_BasicConversion) {
    std::tm tm_info = {};
    tm_info.tm_year = 124; // 2024
    tm_info.tm_mon = 5;    // Июнь (0-based)
    tm_info.tm_mday = 15;
    tm_info.tm_hour = 14;
    tm_info.tm_min = 30;
    tm_info.tm_sec = 45;
    tm_info.tm_isdst = 0;

    Time t = Time::from_tm(tm_info, 120);
    EXPECT_EQ(t.hours(), 14);
    EXPECT_EQ(t.minutes(), 30);
    EXPECT_EQ(t.seconds(), 45);
    EXPECT_FALSE(t.is_dst());
}

TEST_F(TimeTest, ToTm_BasicConversion) {
    Time t(2024, 6, 15, 52245000); // 14:30:45
    std::tm tm_info = t.to_tm();

    EXPECT_EQ(tm_info.tm_year, 124); // 2024 - 1900
    EXPECT_EQ(tm_info.tm_mon, 5);    // Июнь (0-based)
    EXPECT_EQ(tm_info.tm_mday, 15);
    EXPECT_EQ(tm_info.tm_hour, 14);
    EXPECT_EQ(tm_info.tm_min, 30);
    EXPECT_EQ(tm_info.tm_sec, 45);
}

TEST_F(TimeTest, FromTm_ToTm_RoundTrip) {
    std::tm original = {};
    original.tm_year = 124;
    original.tm_mon = 5;
    original.tm_mday = 15;
    original.tm_hour = 14;
    original.tm_min = 30;
    original.tm_sec = 45;
    original.tm_isdst = 1;

    Time t = Time::from_tm(original);
    std::tm result = t.to_tm();

    EXPECT_EQ(original.tm_year, result.tm_year);
    EXPECT_EQ(original.tm_mon, result.tm_mon);
    EXPECT_EQ(original.tm_mday, result.tm_mday);
    EXPECT_EQ(original.tm_hour, result.tm_hour);
    EXPECT_EQ(original.tm_min, result.tm_min);
    EXPECT_EQ(original.tm_sec, result.tm_sec);
    EXPECT_EQ(original.tm_isdst, result.tm_isdst);
}

// ========== Тесты from_time_t и to_time_t ==========

TEST_F(TimeTest, FromTimeT_Epoch) {
    std::time_t epoch = 0;
    Time t = Time::from_time_t(epoch);
    EXPECT_EQ(t.hours(), 0);
    EXPECT_EQ(t.minutes(), 0);
    EXPECT_EQ(t.seconds(), 0);
}

TEST_F(TimeTest, FromTimeT_ToTimeT_RoundTrip) {
    std::time_t original = 1718461845; // Произвольная дата
    Time t = Time::from_time_t(original);
    std::time_t result = t.to_time_t();
    EXPECT_EQ(original, result);
}

// ========== Тесты сериализации ==========

TEST_F(TimeTest, Serialization_ToBytes_FromBytes) {
    Time original(2024, 12, 25, 45296789, 120, 1, 1, 500, 0);
    uint8_t buffer[sizeof(Time)];

    original.to_bytes(buffer);
    Time deserialized = Time::from_bytes(buffer);

    EXPECT_EQ(original, deserialized);
}

TEST_F(TimeTest, Serialization_DefaultTime) {
    Time original;
    uint8_t buffer[sizeof(Time)];

    original.to_bytes(buffer);
    Time deserialized = Time::from_bytes(buffer);

    EXPECT_EQ(original, deserialized);
}

// ========== Тесты операторов сравнения ==========

TEST_F(TimeTest, Equality_SameTimes) {
    Time t1(2024, 6, 15, 45000000);
    Time t2(2024, 6, 15, 45000000);
    EXPECT_TRUE(t1 == t2);
    EXPECT_FALSE(t1 != t2);
}

TEST_F(TimeTest, Equality_DifferentTimes) {
    Time t1(2024, 6, 15, 45000000);
    Time t2(2024, 6, 15, 45000001);
    EXPECT_FALSE(t1 == t2);
    EXPECT_TRUE(t1 != t2);
}

TEST_F(TimeTest, LessThan_DifferentYears) {
    Time t1(2023, 6, 15, 0);
    Time t2(2024, 6, 15, 0);
    EXPECT_TRUE(t1 < t2);
    EXPECT_FALSE(t2 < t1);
}

TEST_F(TimeTest, LessThan_DifferentMonths) {
    Time t1(2024, 5, 15, 0);
    Time t2(2024, 6, 15, 0);
    EXPECT_TRUE(t1 < t2);
    EXPECT_FALSE(t2 < t1);
}

TEST_F(TimeTest, LessThan_DifferentDays) {
    Time t1(2024, 6, 14, 0);
    Time t2(2024, 6, 15, 0);
    EXPECT_TRUE(t1 < t2);
    EXPECT_FALSE(t2 < t1);
}

TEST_F(TimeTest, LessThan_DifferentMilliseconds) {
    Time t1(2024, 6, 15, 45000000);
    Time t2(2024, 6, 15, 45000001);
    EXPECT_TRUE(t1 < t2);
    EXPECT_FALSE(t2 < t1);
}

TEST_F(TimeTest, LessThan_DifferentMicroseconds) {
    Time t1(2024, 6, 15, 45000000, 0, 0, 0, 100, 0);
    Time t2(2024, 6, 15, 45000000, 0, 0, 0, 200, 0);
    EXPECT_TRUE(t1 < t2);
    EXPECT_FALSE(t2 < t1);
}

TEST_F(TimeTest, LessThan_EqualTimes) {
    Time t1(2024, 6, 15, 45000000);
    Time t2(2024, 6, 15, 45000000);
    EXPECT_FALSE(t1 < t2);
    EXPECT_FALSE(t2 < t1);
}

// ========== Тесты строковых представлений ==========

TEST_F(TimeTest, ToString_BasicFormat) {
    Time t(2024, 12, 25, 45296789, 120, 0, 0, 500, 0); // 12:34:56.789
    std::string result = t.to_string();
    EXPECT_NE(result.find("2024-12-25"), std::string::npos);
    EXPECT_NE(result.find("12:34:56"), std::string::npos);
    EXPECT_NE(result.find("789"), std::string::npos);
    EXPECT_NE(result.find("500"), std::string::npos);
    EXPECT_NE(result.find("UTC+02:00"), std::string::npos);
}

TEST_F(TimeTest, ToString_WithLeapSecond) {
    Time t(2024, 6, 15, 45000000, 0, 1, 0, 0, 0);
    std::string result = t.to_string();
    EXPECT_NE(result.find("+leap"), std::string::npos);
}

TEST_F(TimeTest, ToString_WithDST) {
    Time t(2024, 6, 15, 45000000, 0, 0, 1, 0, 0);
    std::string result = t.to_string();
    EXPECT_NE(result.find("DST"), std::string::npos);
}

TEST_F(TimeTest, ToISO8601_BasicFormat) {
    Time t(2024, 12, 25, 45296789, 120, 0, 0, 0, 0); // 12:34:56.789
    std::string result = t.to_iso8601();
    EXPECT_EQ(result, "2024-12-25T12:34:56.789+02:00");
}

TEST_F(TimeTest, ToISO8601_Midnight) {
    Time t(2024, 1, 1, 0, 0, 0, 0, 0, 0);
    std::string result = t.to_iso8601();
    EXPECT_EQ(result, "2024-01-01T00:00:00.000+00:00");
}

TEST_F(TimeTest, ToISO8601_NegativeTimezone) {
    Time t(2024, 6, 15, 45000000, -300, 0, 0, 0, 0);
    std::string result = t.to_iso8601();
    EXPECT_NE(result.find("-05:00"), std::string::npos);
}

// ========== Граничные случаи ==========

TEST_F(TimeTest, EdgeCase_LeapYear_Feb29) {
    Time t(2024, 2, 29, 0); // 2024 - високосный год
    EXPECT_EQ(t.validate(), "");
}

TEST_F(TimeTest, EdgeCase_MaxValidTimezone) {
    Time t(2024, 6, 15, 0, 840); // UTC+14:00
    EXPECT_EQ(t.validate(), "");
}

TEST_F(TimeTest, EdgeCase_MinValidTimezone) {
    Time t(2024, 6, 15, 0, -720); // UTC-12:00
    EXPECT_EQ(t.validate(), "");
}

TEST_F(TimeTest, EdgeCase_MaxMillisecondsWithoutLeapSecond) {
    Time t(2024, 6, 15, 86399999, 0, 0, 0, 0, 0); // 23:59:59.999
    EXPECT_EQ(t.validate(), "");
}

TEST_F(TimeTest, EdgeCase_MaxMicroseconds) {
    Time t(2024, 6, 15, 0, 0, 0, 0, 999, 0);
    EXPECT_EQ(t.validate(), "");
}

// ========== Тесты для работы с system_clock ==========

TEST_F(TimeTest, FromSystemClock_Now) {
    auto now = std::chrono::system_clock::now();
    Time t = Time::from_system_clock(now);
    // Проверяем, что время разумное (не epoch и не далекое будущее)
    EXPECT_GT(t.to_time_t(), 1000000000); // После 2001 года
    EXPECT_LT(t.to_time_t(), 2000000000); // До 2033 года
}

TEST_F(TimeTest, FromSystemClock_Epoch) {
    auto epoch = std::chrono::system_clock::time_point();
    Time t = Time::from_system_clock(epoch);
    EXPECT_EQ(t.hours(), 0);
    EXPECT_EQ(t.minutes(), 0);
    EXPECT_EQ(t.seconds(), 0);
    EXPECT_EQ(t.milliseconds(), 0);
}

TEST_F(TimeTest, FromSystemClock_WithTimezone) {
    auto now = std::chrono::system_clock::now();
    Time t = Time::from_system_clock(now, 180); // UTC+3
    // Проверяем, что часовой пояс установлен (но не проверяем само время)
    EXPECT_EQ(t.validate(), "");
}

TEST_F(TimeTest, FromSystemClock_WithMicroseconds) {
    // Создаем time_point с определенными микросекундами
    auto epoch = std::chrono::system_clock::time_point();
    auto tp = epoch + std::chrono::seconds(1) +
        std::chrono::milliseconds(500) +
        std::chrono::microseconds(750);
    Time t = Time::from_system_clock(tp);
    EXPECT_EQ(t.seconds(), 1);
    EXPECT_EQ(t.milliseconds(), 500);
    EXPECT_EQ(t.microseconds(), 750);
}

TEST_F(TimeTest, ToSystemClock_BasicConversion) {
    Time t(2024, 6, 15, 52245000); // 14:30:45.000
    auto tp = t.to_system_clock();
    std::time_t result_time = std::chrono::system_clock::to_time_t(tp);
    std::time_t expected_time = t.to_time_t();
    EXPECT_EQ(result_time, expected_time);
}

TEST_F(TimeTest, ToSystemClock_WithMilliseconds) {
    Time t(2024, 6, 15, 52245500); // 14:30:45.500
    auto tp = t.to_system_clock();
    auto duration = tp.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    EXPECT_EQ(millis % 1000, 500);
}

TEST_F(TimeTest, ToSystemClock_WithMicroseconds) {
    Time t(2024, 6, 15, 52245500, 0, 0, 0, 750, 0); // 14:30:45.500750
    auto tp = t.to_system_clock();
    auto duration = tp.time_since_epoch();
    auto micros = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    EXPECT_EQ(micros % 1000, 750);
}

TEST_F(TimeTest, SystemClock_RoundTrip) {
    auto original = std::chrono::system_clock::now();
    // system_clock -> Time -> system_clock
    Time t = Time::from_system_clock(original);
    auto result = t.to_system_clock();
    // Разница должна быть меньше 1 микросекунды (из-за округления)
    auto diff = std::chrono::duration_cast<std::chrono::microseconds>(
        result - original
    ).count();
    EXPECT_LE(std::abs(diff), 1);
}

TEST_F(TimeTest, SystemClock_RoundTrip_Reverse) {
    Time original(2024, 6, 15, 52245789, 0, 0, 0, 500, 0);
    // Time -> system_clock -> Time
    auto tp = original.to_system_clock();
    Time result = Time::from_system_clock(tp);
    EXPECT_EQ(original.hours(), result.hours());
    EXPECT_EQ(original.minutes(), result.minutes());
    EXPECT_EQ(original.seconds(), result.seconds());
    EXPECT_EQ(original.milliseconds(), result.milliseconds());
    EXPECT_EQ(original.microseconds(), result.microseconds());
}

TEST_F(TimeTest, Constructor_FromSystemClock) {
    auto now = std::chrono::system_clock::now();
    Time t(now);
    // Проверяем, что объект создан корректно
    EXPECT_EQ(t.validate(), "");
    EXPECT_GT(t.to_time_t(), 1000000000);
}

TEST_F(TimeTest, Constructor_FromSystemClock_WithTimezone) {
    auto now = std::chrono::system_clock::now();
    Time t(now, 300); // UTC+5
    EXPECT_EQ(t.validate(), "");
}

TEST_F(TimeTest, SystemClock_Precision) {
    // Создаем время с точностью до микросекунд
    auto epoch = std::chrono::system_clock::time_point();
    auto tp = epoch + std::chrono::hours(12) +
        std::chrono::minutes(34) +
        std::chrono::seconds(56) +
        std::chrono::milliseconds(789) +
        std::chrono::microseconds(123);
    Time t = Time::from_system_clock(tp);
    EXPECT_EQ(t.hours(), 12);
    EXPECT_EQ(t.minutes(), 34);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 789);
    EXPECT_EQ(t.microseconds(), 123);
}