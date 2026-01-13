#include <gtest/gtest.h>
#include "../time.h"

using namespace dungeons;

class TimeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Общая инициализация
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
    EXPECT_TRUE(t.validate().is_ok());
}

TEST_F(TimeTest, CreateWithBasicParams) {
    auto result = Time::create(2024, 12, 25, 45296789); // 12:34:56.789
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    EXPECT_EQ(t.hours(), 12);
    EXPECT_EQ(t.minutes(), 34);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 789);
}

TEST_F(TimeTest, CreateWithTimezoneOffset) {
    auto result = Time::create(2024, 12, 25, 45296789, 180); // 12:34:56.789 UTC+03
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    EXPECT_EQ(t.hours(), 12);
    EXPECT_EQ(t.minutes(), 34);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 789);
    EXPECT_EQ(t.timezone_offset_minutes(), 180);
}

TEST_F(TimeTest, CreateWithAllParams) {
    auto result = Time::create(2024, 12, 25, 45296789, 120, 1, 1, 500, 0);
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    EXPECT_EQ(t.hours(), 12);
    EXPECT_EQ(t.minutes(), 34);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 789);
    EXPECT_TRUE(t.leap_second());
    EXPECT_TRUE(t.is_dst());
    EXPECT_EQ(t.microseconds(), 500);
}

TEST_F(TimeTest, CreateWithInvalidMonth) {
    auto result = Time::create(2024, 0, 15, 0);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(TimeTest, CreateWithInvalidDay) {
    auto result = Time::create(2024, 6, 32, 0);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

// ========== Тесты геттеров времени ==========

TEST_F(TimeTest, TimeComponents_Midnight) {
    auto result = Time::create(2024, 1, 1, 0);
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    EXPECT_EQ(t.hours(), 0);
    EXPECT_EQ(t.minutes(), 0);
    EXPECT_EQ(t.seconds(), 0);
    EXPECT_EQ(t.milliseconds(), 0);
}

TEST_F(TimeTest, TimeComponents_Noon) {
    auto result = Time::create(2024, 1, 1, 43200000); // 12:00:00.000
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    EXPECT_EQ(t.hours(), 12);
    EXPECT_EQ(t.minutes(), 0);
    EXPECT_EQ(t.seconds(), 0);
    EXPECT_EQ(t.milliseconds(), 0);
}

TEST_F(TimeTest, TimeComponents_AlmostMidnight) {
    auto result = Time::create(2024, 1, 1, 86399999); // 23:59:59.999
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    EXPECT_EQ(t.hours(), 23);
    EXPECT_EQ(t.minutes(), 59);
    EXPECT_EQ(t.seconds(), 59);
    EXPECT_EQ(t.milliseconds(), 999);
}

TEST_F(TimeTest, TimeComponents_ArbitraryTime) {
    auto result = Time::create(2024, 6, 15, 52875432); // 14:41:15.432
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    EXPECT_EQ(t.hours(), 14);
    EXPECT_EQ(t.minutes(), 41);
    EXPECT_EQ(t.seconds(), 15);
    EXPECT_EQ(t.milliseconds(), 432);
}

// ========== Тесты сеттеров ==========

TEST_F(TimeTest, SetMillisecondsFromMidnight_Valid) {
    Time t;
    auto result = t.milliseconds_from_midnight(45296789);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(t.hours(), 12);
    EXPECT_EQ(t.minutes(), 34);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 789);
}

TEST_F(TimeTest, SetMillisecondsFromMidnight_Invalid) {
    Time t;
    auto result = t.milliseconds_from_midnight(86400000);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
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
    auto result = t.set_microseconds(500);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(t.microseconds(), 500);

    result = t.set_microseconds(999);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(t.microseconds(), 999);

    result = t.set_microseconds(0);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(t.microseconds(), 0);
}

TEST_F(TimeTest, SetMicroseconds_Invalid) {
    Time t;
    auto result = t.set_microseconds(500);
    EXPECT_TRUE(result.is_ok());

    result = t.set_microseconds(1000);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
    EXPECT_EQ(t.microseconds(), 500); // Должно остаться прежнее значение

    result = t.set_microseconds(1500);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(t.microseconds(), 500);
}

// ========== Тесты сеттеров компонентов времени ==========

TEST_F(TimeTest, SetHours_Valid) {
    auto create_result = Time::create(2024, 6, 15, 45296789);
    ASSERT_TRUE(create_result.is_ok());
    auto t = create_result.value();

    auto result = t.hours(18);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(t.hours(), 18);
    EXPECT_EQ(t.minutes(), 34);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 789);
}

TEST_F(TimeTest, SetHours_Invalid) {
    auto create_result = Time::create(2024, 6, 15, 45296789);
    ASSERT_TRUE(create_result.is_ok());
    auto t = create_result.value();

    auto result = t.hours(24);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
    EXPECT_EQ(t.hours(), 12); // Должно остаться прежнее значение
}

TEST_F(TimeTest, SetHours_Boundary) {
    auto create_result = Time::create(2024, 6, 15, 45296789);
    ASSERT_TRUE(create_result.is_ok());
    auto t = create_result.value();

    auto result = t.hours(0);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(t.hours(), 0);

    result = t.hours(23);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(t.hours(), 23);
}

TEST_F(TimeTest, SetMinutes_Valid) {
    auto create_result = Time::create(2024, 6, 15, 45296789);
    ASSERT_TRUE(create_result.is_ok());
    auto t = create_result.value();

    auto result = t.minutes(45);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(t.hours(), 12);
    EXPECT_EQ(t.minutes(), 45);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 789);
}

TEST_F(TimeTest, SetMinutes_Invalid) {
    auto create_result = Time::create(2024, 6, 15, 45296789);
    ASSERT_TRUE(create_result.is_ok());
    auto t = create_result.value();

    auto result = t.minutes(60);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
    EXPECT_EQ(t.minutes(), 34);
}

TEST_F(TimeTest, SetMinutes_Boundary) {
    auto create_result = Time::create(2024, 6, 15, 45296789);
    ASSERT_TRUE(create_result.is_ok());
    auto t = create_result.value();

    auto result = t.minutes(0);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(t.minutes(), 0);

    result = t.minutes(59);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(t.minutes(), 59);
}

TEST_F(TimeTest, SetSeconds_Valid) {
    auto create_result = Time::create(2024, 6, 15, 45296789);
    ASSERT_TRUE(create_result.is_ok());
    auto t = create_result.value();

    auto result = t.seconds(30);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(t.hours(), 12);
    EXPECT_EQ(t.minutes(), 34);
    EXPECT_EQ(t.seconds(), 30);
    EXPECT_EQ(t.milliseconds(), 789);
}

TEST_F(TimeTest, SetSeconds_Invalid) {
    auto create_result = Time::create(2024, 6, 15, 45296789);
    ASSERT_TRUE(create_result.is_ok());
    auto t = create_result.value();

    auto result = t.seconds(60);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
    EXPECT_EQ(t.seconds(), 56);
}

TEST_F(TimeTest, SetSeconds_Boundary) {
    auto create_result = Time::create(2024, 6, 15, 45296789);
    ASSERT_TRUE(create_result.is_ok());
    auto t = create_result.value();

    auto result = t.seconds(0);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(t.seconds(), 0);

    result = t.seconds(59);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(t.seconds(), 59);
}

TEST_F(TimeTest, SetMilliseconds_Valid) {
    auto create_result = Time::create(2024, 6, 15, 45296789);
    ASSERT_TRUE(create_result.is_ok());
    auto t = create_result.value();

    auto result = t.milliseconds(123);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(t.hours(), 12);
    EXPECT_EQ(t.minutes(), 34);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 123);
}

TEST_F(TimeTest, SetMilliseconds_Invalid) {
    auto create_result = Time::create(2024, 6, 15, 45296789);
    ASSERT_TRUE(create_result.is_ok());
    auto t = create_result.value();

    auto result = t.milliseconds(1000);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
    EXPECT_EQ(t.milliseconds(), 789);
}

TEST_F(TimeTest, SetMilliseconds_Boundary) {
    auto create_result = Time::create(2024, 6, 15, 45296789);
    ASSERT_TRUE(create_result.is_ok());
    auto t = create_result.value();

    auto result = t.milliseconds(0);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(t.milliseconds(), 0);

    result = t.milliseconds(999);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(t.milliseconds(), 999);
}

TEST_F(TimeTest, SetMultipleComponents) {
    auto create_result = Time::create(2024, 6, 15, 0);
    ASSERT_TRUE(create_result.is_ok());
    auto t = create_result.value();

    EXPECT_TRUE(t.hours(15).is_ok());
    EXPECT_TRUE(t.minutes(30).is_ok());
    EXPECT_TRUE(t.seconds(45).is_ok());
    EXPECT_TRUE(t.milliseconds(500).is_ok());

    EXPECT_EQ(t.hours(), 15);
    EXPECT_EQ(t.minutes(), 30);
    EXPECT_EQ(t.seconds(), 45);
    EXPECT_EQ(t.milliseconds(), 500);
}

TEST_F(TimeTest, SetComponents_PreserveOthers) {
    auto create_result = Time::create(2024, 6, 15, 45296789);
    ASSERT_TRUE(create_result.is_ok());
    auto t = create_result.value();

    // Изменяем только часы
    EXPECT_TRUE(t.hours(20).is_ok());
    EXPECT_EQ(t.hours(), 20);
    EXPECT_EQ(t.minutes(), 34);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 789);

    // Изменяем только минуты
    EXPECT_TRUE(t.minutes(15).is_ok());
    EXPECT_EQ(t.hours(), 20);
    EXPECT_EQ(t.minutes(), 15);
    EXPECT_EQ(t.seconds(), 56);
    EXPECT_EQ(t.milliseconds(), 789);
}

TEST_F(TimeTest, SetMonth_Valid) {
    Time t;
    auto result = t.month(6);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(t.month(), 6);
}

TEST_F(TimeTest, SetMonth_Invalid) {
    Time t;
    auto result = t.month(13);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(TimeTest, SetDay_Valid) {
    Time t;
    auto result = t.day(15);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(t.day(), 15);
}

TEST_F(TimeTest, SetDay_Invalid) {
    Time t;
    auto result = t.day(32);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(TimeTest, SetTimezoneOffset_Valid) {
    Time t;
    auto result = t.timezone_offset_minutes(180);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(t.timezone_offset_minutes(), 180);
}

TEST_F(TimeTest, SetTimezoneOffset_Invalid) {
    Time t;
    auto result = t.timezone_offset_minutes(1000);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

// ========== Тесты валидации ==========

TEST_F(TimeTest, Validate_ValidTime) {
    auto result = Time::create(2024, 6, 15, 45000000, 180, 0, 0, 500, 0);
    ASSERT_TRUE(result.is_ok());
    EXPECT_TRUE(result.value().validate().is_ok());
}

TEST_F(TimeTest, Validate_InvalidMonth_TooLow) {
    auto result = Time::create(2024, 0, 15, 0);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(TimeTest, Validate_InvalidMonth_TooHigh) {
    auto result = Time::create(2024, 13, 15, 0);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(TimeTest, Validate_InvalidDay_TooLow) {
    auto result = Time::create(2024, 6, 0, 0);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(TimeTest, Validate_InvalidDay_TooHigh) {
    auto result = Time::create(2024, 6, 32, 0);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(TimeTest, Validate_InvalidMilliseconds_NoLeapSecond) {
    auto result = Time::create(2024, 6, 15, 86400000);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(TimeTest, Validate_ValidMilliseconds_WithLeapSecond) {
    auto result = Time::create(2024, 6, 15, 86400500, 0, 1, 0, 0, 0);
    EXPECT_TRUE(result.is_ok());
}

TEST_F(TimeTest, Validate_InvalidMilliseconds_WithLeapSecond) {
    auto result = Time::create(2024, 6, 15, 86401000, 0, 1, 0, 0, 0);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(TimeTest, Validate_InvalidTimezone_TooLow) {
    auto result = Time::create(2024, 6, 15, 0, -721);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(TimeTest, Validate_InvalidTimezone_TooHigh) {
    auto result = Time::create(2024, 6, 15, 0, 841);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(TimeTest, Validate_ValidTimezone_Boundaries) {
    auto result1 = Time::create(2024, 6, 15, 0, -720);
    EXPECT_TRUE(result1.is_ok());

    auto result2 = Time::create(2024, 6, 15, 0, 840);
    EXPECT_TRUE(result2.is_ok());
}

TEST_F(TimeTest, Validate_InvalidMicroseconds) {
    auto result = Time::create(2024, 6, 15, 0, 0, 0, 0, 1000, 0);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
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

    auto result = Time::from_tm(tm_info, 120);
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    EXPECT_EQ(t.hours(), 14);
    EXPECT_EQ(t.minutes(), 30);
    EXPECT_EQ(t.seconds(), 45);
    EXPECT_FALSE(t.is_dst());
}

TEST_F(TimeTest, ToTm_BasicConversion) {
    auto result = Time::create(2024, 6, 15, 52245000);
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
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

    auto time_result = Time::from_tm(original);
    ASSERT_TRUE(time_result.is_ok());
    auto t = time_result.value();
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
    auto result = Time::from_time_t(epoch);
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    EXPECT_EQ(t.hours(), 0);
    EXPECT_EQ(t.minutes(), 0);
    EXPECT_EQ(t.seconds(), 0);
}

TEST_F(TimeTest, FromTimeT_ToTimeT_RoundTrip) {
    std::time_t original = 1718461845;
    auto time_result = Time::from_time_t(original);
    ASSERT_TRUE(time_result.is_ok());
    auto t = time_result.value();
    auto result = t.to_time_t();
    ASSERT_TRUE(result.is_ok());
    EXPECT_EQ(original, result.value());
}

// ========== Тесты сериализации ==========

TEST_F(TimeTest, Serialization_ToBytes_FromBytes) {
    auto create_result = Time::create(2024, 12, 25, 45296789, 120, 1, 1, 500, 0);
    ASSERT_TRUE(create_result.is_ok());
    auto original = create_result.value();
    uint8_t buffer[sizeof(Time)];

    original.to_bytes(buffer);
    auto deser_result = Time::from_bytes(buffer);
    ASSERT_TRUE(deser_result.is_ok());
    auto deserialized = deser_result.value();

    EXPECT_EQ(original, deserialized);
}

TEST_F(TimeTest, Serialization_DefaultTime) {
    Time original;
    uint8_t buffer[sizeof(Time)];

    original.to_bytes(buffer);
    auto result = Time::from_bytes(buffer);
    ASSERT_TRUE(result.is_ok());
    auto deserialized = result.value();

    EXPECT_EQ(original, deserialized);
}

TEST_F(TimeTest, Serialization_NullBuffer) {
    auto result = Time::from_bytes(nullptr);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::INVALID_ARGUMENT);
}

// ========== Тесты операторов сравнения ==========

TEST_F(TimeTest, Equality_SameTimes) {
    auto result1 = Time::create(2024, 6, 15, 45000000);
    auto result2 = Time::create(2024, 6, 15, 45000000);
    ASSERT_TRUE(result1.is_ok());
    ASSERT_TRUE(result2.is_ok());
    auto t1 = result1.value();
    auto t2 = result2.value();
    EXPECT_TRUE(t1 == t2);
    EXPECT_FALSE(t1 != t2);
}

TEST_F(TimeTest, Equality_DifferentTimes) {
    auto result1 = Time::create(2024, 6, 15, 45000000);
    auto result2 = Time::create(2024, 6, 15, 45000001);
    ASSERT_TRUE(result1.is_ok());
    ASSERT_TRUE(result2.is_ok());
    auto t1 = result1.value();
    auto t2 = result2.value();
    EXPECT_FALSE(t1 == t2);
    EXPECT_TRUE(t1 != t2);
}

TEST_F(TimeTest, LessThan_DifferentYears) {
    auto result1 = Time::create(2023, 6, 15, 0);
    auto result2 = Time::create(2024, 6, 15, 0);
    ASSERT_TRUE(result1.is_ok());
    ASSERT_TRUE(result2.is_ok());
    auto t1 = result1.value();
    auto t2 = result2.value();
    EXPECT_TRUE(t1 < t2);
    EXPECT_FALSE(t2 < t1);
}

TEST_F(TimeTest, LessThan_DifferentMonths) {
    auto result1 = Time::create(2024, 5, 15, 0);
    auto result2 = Time::create(2024, 6, 15, 0);
    ASSERT_TRUE(result1.is_ok());
    ASSERT_TRUE(result2.is_ok());
    auto t1 = result1.value();
    auto t2 = result2.value();
    EXPECT_TRUE(t1 < t2);
    EXPECT_FALSE(t2 < t1);
}

TEST_F(TimeTest, LessThan_DifferentDays) {
    auto result1 = Time::create(2024, 6, 14, 0);
    auto result2 = Time::create(2024, 6, 15, 0);
    ASSERT_TRUE(result1.is_ok());
    ASSERT_TRUE(result2.is_ok());
    auto t1 = result1.value();
    auto t2 = result2.value();
    EXPECT_TRUE(t1 < t2);
    EXPECT_FALSE(t2 < t1);
}

TEST_F(TimeTest, LessThan_DifferentMilliseconds) {
    auto result1 = Time::create(2024, 6, 15, 45000000);
    auto result2 = Time::create(2024, 6, 15, 45000001);
    ASSERT_TRUE(result1.is_ok());
    ASSERT_TRUE(result2.is_ok());
    auto t1 = result1.value();
    auto t2 = result2.value();
    EXPECT_TRUE(t1 < t2);
    EXPECT_FALSE(t2 < t1);
}

TEST_F(TimeTest, LessThan_DifferentMicroseconds) {
    auto result1 = Time::create(2024, 6, 15, 45000000, 0, 0, 0, 100, 0);
    auto result2 = Time::create(2024, 6, 15, 45000000, 0, 0, 0, 200, 0);
    ASSERT_TRUE(result1.is_ok());
    ASSERT_TRUE(result2.is_ok());
    auto t1 = result1.value();
    auto t2 = result2.value();
    EXPECT_TRUE(t1 < t2);
    EXPECT_FALSE(t2 < t1);
}

TEST_F(TimeTest, LessThan_EqualTimes) {
    auto result1 = Time::create(2024, 6, 15, 45000000);
    auto result2 = Time::create(2024, 6, 15, 45000000);
    ASSERT_TRUE(result1.is_ok());
    ASSERT_TRUE(result2.is_ok());
    auto t1 = result1.value();
    auto t2 = result2.value();
    EXPECT_FALSE(t1 < t2);
    EXPECT_FALSE(t2 < t1);
}

TEST_F(TimeTest, ComparisonOperators_LessEqual) {
    auto result1 = Time::create(2024, 6, 15, 45000000);
    auto result2 = Time::create(2024, 6, 15, 45000001);
    auto result3 = Time::create(2024, 6, 15, 45000000);
    ASSERT_TRUE(result1.is_ok());
    ASSERT_TRUE(result2.is_ok());
    ASSERT_TRUE(result3.is_ok());
    auto t1 = result1.value();
    auto t2 = result2.value();
    auto t3 = result3.value();

    EXPECT_TRUE(t1 <= t2);
    EXPECT_TRUE(t1 <= t3);
    EXPECT_FALSE(t2 <= t1);
}

TEST_F(TimeTest, ComparisonOperators_Greater) {
    auto result1 = Time::create(2024, 6, 15, 45000001);
    auto result2 = Time::create(2024, 6, 15, 45000000);
    ASSERT_TRUE(result1.is_ok());
    ASSERT_TRUE(result2.is_ok());
    auto t1 = result1.value();
    auto t2 = result2.value();

    EXPECT_TRUE(t1 > t2);
    EXPECT_FALSE(t2 > t1);
}

TEST_F(TimeTest, ComparisonOperators_GreaterEqual) {
    auto result1 = Time::create(2024, 6, 15, 45000001);
    auto result2 = Time::create(2024, 6, 15, 45000000);
    auto result3 = Time::create(2024, 6, 15, 45000001);
    ASSERT_TRUE(result1.is_ok());
    ASSERT_TRUE(result2.is_ok());
    ASSERT_TRUE(result3.is_ok());
    auto t1 = result1.value();
    auto t2 = result2.value();
    auto t3 = result3.value();

    EXPECT_TRUE(t1 >= t2);
    EXPECT_TRUE(t1 >= t3);
    EXPECT_FALSE(t2 >= t1);
}

// ========== Тесты строковых представлений ==========

TEST_F(TimeTest, ToString_BasicFormat) {
    auto result = Time::create(2024, 12, 25, 45296789, 120, 0, 0, 500, 0);
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    std::string str_result = t.to_string();
    EXPECT_NE(str_result.find("2024-12-25"), std::string::npos);
    EXPECT_NE(str_result.find("12:34:56"), std::string::npos);
    EXPECT_NE(str_result.find("789"), std::string::npos);
    EXPECT_NE(str_result.find("500"), std::string::npos);
    EXPECT_NE(str_result.find("UTC+02:00"), std::string::npos);
}

TEST_F(TimeTest, ToString_WithLeapSecond) {
    auto result = Time::create(2024, 6, 15, 45000000, 0, 1, 0, 0, 0);
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    std::string str_result = t.to_string();
    EXPECT_NE(str_result.find("+leap"), std::string::npos);
}

TEST_F(TimeTest, ToString_WithDST) {
    auto result = Time::create(2024, 6, 15, 45000000, 0, 0, 1, 0, 0);
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    std::string str_result = t.to_string();
    EXPECT_NE(str_result.find("DST"), std::string::npos);
}

TEST_F(TimeTest, ToISO8601_BasicFormat) {
    auto result = Time::create(2024, 12, 25, 45296789, 120, 0, 0, 0, 0);
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    std::string str_result = t.to_iso8601();
    EXPECT_EQ(str_result, "2024-12-25T12:34:56.789+02:00");
}

TEST_F(TimeTest, ToISO8601_Midnight) {
    auto result = Time::create(2024, 1, 1, 0, 0, 0, 0, 0, 0);
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    std::string str_result = t.to_iso8601();
    EXPECT_EQ(str_result, "2024-01-01T00:00:00.000+00:00");
}

TEST_F(TimeTest, ToISO8601_NegativeTimezone) {
    auto result = Time::create(2024, 6, 15, 45000000, -300, 0, 0, 0, 0);
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    std::string str_result = t.to_iso8601();
    EXPECT_NE(str_result.find("-05:00"), std::string::npos);
}

// ========== Граничные случаи ==========

TEST_F(TimeTest, EdgeCase_LeapYear_Feb29) {
    auto result = Time::create(2024, 2, 29, 0);
    EXPECT_TRUE(result.is_ok());
}

TEST_F(TimeTest, EdgeCase_MaxValidTimezone) {
    auto result = Time::create(2024, 6, 15, 0, 840);
    EXPECT_TRUE(result.is_ok());
}

TEST_F(TimeTest, EdgeCase_MinValidTimezone) {
    auto result = Time::create(2024, 6, 15, 0, -720);
    EXPECT_TRUE(result.is_ok());
}

TEST_F(TimeTest, EdgeCase_MaxMillisecondsWithoutLeapSecond) {
    auto result = Time::create(2024, 6, 15, 86399999, 0, 0, 0, 0, 0);
    EXPECT_TRUE(result.is_ok());
}

TEST_F(TimeTest, EdgeCase_MaxMicroseconds) {
    auto result = Time::create(2024, 6, 15, 0, 0, 0, 0, 999, 0);
    EXPECT_TRUE(result.is_ok());
}

// ========== Тесты для работы с system_clock ==========

TEST_F(TimeTest, FromSystemClock_Now) {
    auto now = std::chrono::system_clock::now();
    auto result = Time::from_system_clock(now);
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    auto time_t_result = t.to_time_t();
    ASSERT_TRUE(time_t_result.is_ok());
    EXPECT_GT(time_t_result.value(), 1000000000);
    EXPECT_LT(time_t_result.value(), 2000000000);
}

TEST_F(TimeTest, FromSystemClock_Epoch) {
    auto epoch = std::chrono::system_clock::time_point();
    auto result = Time::from_system_clock(epoch);
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    EXPECT_EQ(t.hours(), 0);
    EXPECT_EQ(t.minutes(), 0);
    EXPECT_EQ(t.seconds(), 0);
    EXPECT_EQ(t.milliseconds(), 0);
}

TEST_F(TimeTest, FromSystemClock_WithTimezone) {
    auto now = std::chrono::system_clock::now();
    auto result = Time::from_system_clock(now, 180);
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    EXPECT_TRUE(t.validate().is_ok());
    EXPECT_EQ(t.timezone_offset_minutes(), 180);
}

TEST_F(TimeTest, FromSystemClock_WithMicroseconds) {
    auto epoch = std::chrono::system_clock::time_point();
    auto tp = epoch + std::chrono::seconds(1) +
        std::chrono::milliseconds(500) +
        std::chrono::microseconds(750);
    auto result = Time::from_system_clock(tp);
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    EXPECT_EQ(t.seconds(), 1);
    EXPECT_EQ(t.milliseconds(), 500);
    EXPECT_EQ(t.microseconds(), 750);
}

TEST_F(TimeTest, ToSystemClock_BasicConversion) {
    auto create_result = Time::create(2024, 6, 15, 52245000);
    ASSERT_TRUE(create_result.is_ok());
    auto t = create_result.value();
    auto tp_result = t.to_system_clock();
    ASSERT_TRUE(tp_result.is_ok());
    auto tp = tp_result.value();
    std::time_t result_time = std::chrono::system_clock::to_time_t(tp);
    auto expected_time_result = t.to_time_t();
    ASSERT_TRUE(expected_time_result.is_ok());
    std::time_t expected_time = expected_time_result.value();
    EXPECT_EQ(result_time, expected_time);
}

TEST_F(TimeTest, ToSystemClock_WithMilliseconds) {
    auto create_result = Time::create(2024, 6, 15, 52245500);
    ASSERT_TRUE(create_result.is_ok());
    auto t = create_result.value();
    auto tp_result = t.to_system_clock();
    ASSERT_TRUE(tp_result.is_ok());
    auto tp = tp_result.value();
    auto duration = tp.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    EXPECT_EQ(millis % 1000, 500);
}

TEST_F(TimeTest, ToSystemClock_WithMicroseconds) {
    auto create_result = Time::create(2024, 6, 15, 52245500, 0, 0, 0, 750, 0);
    ASSERT_TRUE(create_result.is_ok());
    auto t = create_result.value();
    auto tp_result = t.to_system_clock();
    ASSERT_TRUE(tp_result.is_ok());
    auto tp = tp_result.value();
    auto duration = tp.time_since_epoch();
    auto micros = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    EXPECT_EQ(micros % 1000, 750);
}

TEST_F(TimeTest, SystemClock_RoundTrip) {
    auto original = std::chrono::system_clock::now();
    auto time_result = Time::from_system_clock(original);
    ASSERT_TRUE(time_result.is_ok());
    auto t = time_result.value();
    auto result = t.to_system_clock();
    ASSERT_TRUE(result.is_ok());
    auto tp = result.value();
    auto diff = std::chrono::duration_cast<std::chrono::microseconds>(
        tp - original
    ).count();
    EXPECT_LE(std::abs(diff), 1);
}

TEST_F(TimeTest, SystemClock_RoundTrip_Reverse) {
    auto create_result = Time::create(2024, 6, 15, 52245789, 0, 0, 0, 500, 0);
    ASSERT_TRUE(create_result.is_ok());
    auto original = create_result.value();
    auto tp_result = original.to_system_clock();
    ASSERT_TRUE(tp_result.is_ok());
    auto tp = tp_result.value();
    auto result = Time::from_system_clock(tp);
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    EXPECT_EQ(original.hours(), t.hours());
    EXPECT_EQ(original.minutes(), t.minutes());
    EXPECT_EQ(original.seconds(), t.seconds());
    EXPECT_EQ(original.milliseconds(), t.milliseconds());
    EXPECT_EQ(original.microseconds(), t.microseconds());
}

// ========== Тесты now() ==========

TEST_F(TimeTest, Now_ReturnsValidTime) {
    auto result = Time::now();
    ASSERT_TRUE(result.is_ok());
    auto t = result.value();
    EXPECT_TRUE(t.validate().is_ok());
    auto time_t_result = t.to_time_t();
    ASSERT_TRUE(time_t_result.is_ok());
    EXPECT_GT(time_t_result.value(), 1000000000);
}

// ========== Тесты move_to_timezone ==========

TEST_F(TimeTest, MoveToTimezone_Valid) {
    auto create_result = Time::create(2024, 6, 15, 43200000, 0); // 12:00:00 UTC
    ASSERT_TRUE(create_result.is_ok());
    auto t = create_result.value();

    auto result = t.move_to_timezone(180); // UTC+3
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(t.timezone_offset_minutes(), 180);
}

TEST_F(TimeTest, MoveToTimezone_Invalid) {
    auto create_result = Time::create(2024, 6, 15, 43200000, 0);
    ASSERT_TRUE(create_result.is_ok());
    auto t = create_result.value();

    auto result = t.move_to_timezone(1000);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}