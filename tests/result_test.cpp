#include "../result.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace dungeons;

// ============================================================================
// Тесты для Error
// ============================================================================

TEST(ErrorTest, DefaultConstructor) {
    Error err;
    EXPECT_EQ(err.code(), ErrorCode::OK);
    EXPECT_TRUE(err.message().empty());
    EXPECT_TRUE(err.is_ok());
}

TEST(ErrorTest, ConstructorWithCodeAndMessage) {
    Error err(ErrorCode::INVALID_ARGUMENT, "Test error");
    EXPECT_EQ(err.code(), ErrorCode::INVALID_ARGUMENT);
    EXPECT_EQ(err.message(), "Test error");
    EXPECT_FALSE(err.is_ok());
}

TEST(ErrorTest, IsOkVariousCodes) {
    EXPECT_TRUE(Error(ErrorCode::OK, "").is_ok());
    EXPECT_FALSE(Error(ErrorCode::INVALID_ARGUMENT, "").is_ok());
    EXPECT_FALSE(Error(ErrorCode::OUT_OF_RANGE, "").is_ok());
    EXPECT_FALSE(Error(ErrorCode::EMPTY_CONTAINER, "").is_ok());
    EXPECT_FALSE(Error(ErrorCode::VALIDATION_FAILED, "").is_ok());
    EXPECT_FALSE(Error(ErrorCode::INVALID_DIMENSIONS, "").is_ok());
    EXPECT_FALSE(Error(ErrorCode::NOT_IMPLEMENTED, "").is_ok());
}

TEST(ErrorTest, MessagePreservation) {
    std::string long_message = "This is a very long error message with lots of details about what went wrong";
    Error err(ErrorCode::VALIDATION_FAILED, long_message);
    EXPECT_EQ(err.message(), long_message);
}

// ============================================================================
// Тесты для Result<T> - базовая функциональность
// ============================================================================

TEST(ResultTest, SuccessConstructor) {
    Result<int> result(42);
    EXPECT_TRUE(result.is_ok());
    EXPECT_FALSE(result.is_error());
    EXPECT_EQ(result.value(), 42);
    EXPECT_TRUE(static_cast<bool>(result));
}

TEST(ResultTest, ErrorConstructor) {
    Result<int> result(Error(ErrorCode::INVALID_ARGUMENT, "Invalid value"));
    EXPECT_FALSE(result.is_ok());
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::INVALID_ARGUMENT);
    EXPECT_EQ(result.error().message(), "Invalid value");
    EXPECT_FALSE(static_cast<bool>(result));
}

TEST(ResultTest, ErrorConstructorWithCodeAndMessage) {
    Result<int> result(ErrorCode::OUT_OF_RANGE, "Index out of bounds");
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
    EXPECT_EQ(result.error().message(), "Index out of bounds");
}

// ============================================================================
// Тесты для Result<T> - различные типы
// ============================================================================

TEST(ResultTest, StringType) {
    Result<std::string> result("Hello, World!");
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(result.value(), "Hello, World!");
}

TEST(ResultTest, VectorType) {
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    Result<std::vector<int>> result(vec);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(result.value().size(), 5);
    EXPECT_EQ(result.value()[2], 3);
}

TEST(ResultTest, PointerType) {
    int value = 42;
    Result<int*> result(&value);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(*result.value(), 42);
}

struct CustomStruct {
    int x;
    std::string y;

    bool operator==(const CustomStruct& other) const {
        return x == other.x && y == other.y;
    }
};

TEST(ResultTest, CustomStructType) {
    CustomStruct data{ 100, "test" };
    Result<CustomStruct> result(data);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(result.value().x, 100);
    EXPECT_EQ(result.value().y, "test");
}

// ============================================================================
// Тесты для Result<T> - копирование и перемещение
// ============================================================================

TEST(ResultTest, CopyConstructorSuccess) {
    Result<int> original(42);
    Result<int> copy(original);
    EXPECT_TRUE(copy.is_ok());
    EXPECT_EQ(copy.value(), 42);
    EXPECT_TRUE(original.is_ok());
    EXPECT_EQ(original.value(), 42);
}

TEST(ResultTest, CopyConstructorError) {
    Result<int> original(ErrorCode::INVALID_ARGUMENT, "Error");
    Result<int> copy(original);
    EXPECT_TRUE(copy.is_error());
    EXPECT_EQ(copy.error().code(), ErrorCode::INVALID_ARGUMENT);
    EXPECT_EQ(copy.error().message(), "Error");
}

TEST(ResultTest, MoveConstructorSuccess) {
    Result<std::string> original("Move me");
    Result<std::string> moved(std::move(original));
    EXPECT_TRUE(moved.is_ok());
    EXPECT_EQ(moved.value(), "Move me");
}

TEST(ResultTest, CopyAssignmentSuccess) {
    Result<int> original(42);
    Result<int> target(0);
    target = original;
    EXPECT_TRUE(target.is_ok());
    EXPECT_EQ(target.value(), 42);
}

TEST(ResultTest, MoveAssignmentSuccess) {
    Result<std::string> original("Move me");
    Result<std::string> target("Old value");
    target = std::move(original);
    EXPECT_TRUE(target.is_ok());
    EXPECT_EQ(target.value(), "Move me");
}

TEST(ResultTest, AssignErrorToSuccess) {
    Result<int> success(42);
    Result<int> error(ErrorCode::VALIDATION_FAILED, "Failed");
    success = error;
    EXPECT_TRUE(success.is_error());
    EXPECT_EQ(success.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST(ResultTest, AssignSuccessToError) {
    Result<int> error(ErrorCode::VALIDATION_FAILED, "Failed");
    Result<int> success(42);
    error = success;
    EXPECT_TRUE(error.is_ok());
    EXPECT_EQ(error.value(), 42);
}

// ============================================================================
// Тесты для Result<T> - value() различные варианты
// ============================================================================

TEST(ResultTest, ValueLvalueReference) {
    Result<int> result(42);
    int& ref = result.value();
    ref = 100;
    EXPECT_EQ(result.value(), 100);
}

TEST(ResultTest, ValueConstLvalueReference) {
    const Result<int> result(42);
    const int& ref = result.value();
    EXPECT_EQ(ref, 42);
}

TEST(ResultTest, ValueRvalueReference) {
    Result<std::string> result("Move me");
    std::string moved = std::move(result).value();
    EXPECT_EQ(moved, "Move me");
}

TEST(ResultTest, ValueThrowsOnError) {
    Result<int> result(ErrorCode::INVALID_ARGUMENT, "Error");
    EXPECT_THROW(result.value(), std::bad_variant_access);
}

TEST(ResultTest, ErrorThrowsOnSuccess) {
    Result<int> result(42);
    EXPECT_THROW(result.error(), std::bad_variant_access);
}

// ============================================================================
// Тесты для Result<void>
// ============================================================================

TEST(ResultVoidTest, DefaultConstructorSuccess) {
    Result<void> result;
    EXPECT_TRUE(result.is_ok());
    EXPECT_FALSE(result.is_error());
    EXPECT_TRUE(static_cast<bool>(result));
}

TEST(ResultVoidTest, ErrorConstructor) {
    Result<void> result(Error(ErrorCode::NOT_IMPLEMENTED, "Not done yet"));
    EXPECT_FALSE(result.is_ok());
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::NOT_IMPLEMENTED);
    EXPECT_EQ(result.error().message(), "Not done yet");
    EXPECT_FALSE(static_cast<bool>(result));
}

TEST(ResultVoidTest, ErrorConstructorWithCodeAndMessage) {
    Result<void> result(ErrorCode::EMPTY_CONTAINER, "Container is empty");
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::EMPTY_CONTAINER);
    EXPECT_EQ(result.error().message(), "Container is empty");
}

TEST(ResultVoidTest, ErrorAccess) {
    Result<void> result(ErrorCode::VALIDATION_FAILED, "Validation error");
    const Error& err = result.error();
    EXPECT_EQ(err.code(), ErrorCode::VALIDATION_FAILED);
    EXPECT_EQ(err.message(), "Validation error");
}

// ============================================================================
// Тесты для вспомогательных функций Ok()
// ============================================================================

TEST(OkFunctionTest, OkWithInt) {
    auto result = Ok(42);
    static_assert(std::is_same_v<decltype(result), Result<int>>);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(result.value(), 42);
}

TEST(OkFunctionTest, OkWithString) {
    auto result = Ok(std::string("Success"));
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(result.value(), "Success");
}

TEST(OkFunctionTest, OkWithCustomType) {
    CustomStruct data{ 42, "test" };
    auto result = Ok(data);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(result.value().x, 42);
}

TEST(OkFunctionTest, OkVoid) {
    auto result = Ok();
    static_assert(std::is_same_v<decltype(result), Result<void>>);
    EXPECT_TRUE(result.is_ok());
}

// ============================================================================
// Тесты для вспомогательных функций Err()
// ============================================================================

TEST(ErrFunctionTest, ErrWithType) {
    auto result = Err<int>(ErrorCode::INVALID_ARGUMENT, "Bad argument");
    static_assert(std::is_same_v<decltype(result), Result<int>>);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::INVALID_ARGUMENT);
    EXPECT_EQ(result.error().message(), "Bad argument");
}

TEST(ErrFunctionTest, ErrVoid) {
    auto result = Err(ErrorCode::OUT_OF_RANGE, "Out of range");
    static_assert(std::is_same_v<decltype(result), Result<void>>);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST(ErrFunctionTest, ErrDefaultNotImplemented) {
    auto result = Err<int>();
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::NOT_IMPLEMENTED);
    EXPECT_EQ(result.error().message(), "This method or class is yet to be implemented.");
}

TEST(ErrFunctionTest, ErrVoidDefaultNotImplemented) {
    auto result = Err();
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::NOT_IMPLEMENTED);
    EXPECT_EQ(result.error().message(), "This method or class is yet to be implemented.");
}

// ============================================================================
// Тесты для всех кодов ошибок
// ============================================================================

TEST(ErrorCodeTest, AllErrorCodes) {
    std::vector<ErrorCode> codes = {
        ErrorCode::OK,
        ErrorCode::INVALID_ARGUMENT,
        ErrorCode::OUT_OF_RANGE,
        ErrorCode::EMPTY_CONTAINER,
        ErrorCode::VALIDATION_FAILED,
        ErrorCode::INVALID_DIMENSIONS,
        ErrorCode::NOT_IMPLEMENTED
    };

    for (auto code : codes) {
        Error err(code, "Test");
        EXPECT_EQ(err.code(), code);
        if (code == ErrorCode::OK) {
            EXPECT_TRUE(err.is_ok());
        }
        else {
            EXPECT_FALSE(err.is_ok());
        }
    }
}

// ============================================================================
// Интеграционные тесты - реальные сценарии использования
// ============================================================================

Result<int> divide(int a, int b) {
    if (b == 0) {
        return Err<int>(ErrorCode::INVALID_ARGUMENT, "Division by zero");
    }
    return Ok(a / b);
}

TEST(IntegrationTest, DivideSuccess) {
    auto result = divide(10, 2);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(result.value(), 5);
}

TEST(IntegrationTest, DivideByZero) {
    auto result = divide(10, 0);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::INVALID_ARGUMENT);
    EXPECT_EQ(result.error().message(), "Division by zero");
}

Result<std::vector<int>> get_range(int start, int end) {
    if (start > end) {
        return Err<std::vector<int>>(ErrorCode::INVALID_ARGUMENT,
            "Start must be less than or equal to end");
    }

    std::vector<int> result;
    for (int i = start; i <= end; ++i) {
        result.push_back(i);
    }
    return Ok(std::move(result));
}

TEST(IntegrationTest, GetRangeSuccess) {
    auto result = get_range(1, 5);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(result.value().size(), 5);
    EXPECT_EQ(result.value()[0], 1);
    EXPECT_EQ(result.value()[4], 5);
}

TEST(IntegrationTest, GetRangeInvalidRange) {
    auto result = get_range(10, 5);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::INVALID_ARGUMENT);
}

Result<void> validate_positive(int value) {
    if (value <= 0) {
        return Err(ErrorCode::VALIDATION_FAILED, "Value must be positive");
    }
    return Ok();
}

TEST(IntegrationTest, ValidatePositiveSuccess) {
    auto result = validate_positive(10);
    EXPECT_TRUE(result.is_ok());
}

TEST(IntegrationTest, ValidatePositiveFail) {
    auto result = validate_positive(-5);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

// Цепочка операций с Result
Result<int> parse_and_double(const std::string& str) {
    try {
        int value = std::stoi(str);
        return Ok(value * 2);
    }
    catch (...) {
        return Err<int>(ErrorCode::INVALID_ARGUMENT, "Failed to parse string");
    }
}

TEST(IntegrationTest, ChainedOperations) {
    auto result = parse_and_double("42");
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(result.value(), 84);

    auto error_result = parse_and_double("not a number");
    EXPECT_TRUE(error_result.is_error());
    EXPECT_EQ(error_result.error().code(), ErrorCode::INVALID_ARGUMENT);
}

// ============================================================================
// Тесты граничных случаев
// ============================================================================

TEST(EdgeCaseTest, EmptyStringError) {
    Result<int> result(ErrorCode::VALIDATION_FAILED, "");
    EXPECT_TRUE(result.is_error());
    EXPECT_TRUE(result.error().message().empty());
}

TEST(EdgeCaseTest, LargeStringValue) {
    std::string large_string(10000, 'a');
    Result<std::string> result(large_string);
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(result.value().size(), 10000);
}

TEST(EdgeCaseTest, MultipleAssignments) {
    Result<int> result(1);
    result = Ok(2);
    EXPECT_EQ(result.value(), 2);
    result = Err<int>(ErrorCode::INVALID_ARGUMENT, "Error");
    EXPECT_TRUE(result.is_error());
    result = Ok(3);
    EXPECT_EQ(result.value(), 3);
}

TEST(EdgeCaseTest, BoolConversionInIfStatement) {
    auto success = Ok(42);
    auto failure = Err<int>(ErrorCode::INVALID_ARGUMENT, "Error");

    if (success) {
        EXPECT_TRUE(true); // Достигается
    }
    else {
        FAIL() << "Should not reach here";
    }

    if (failure) {
        FAIL() << "Should not reach here";
    }
    else {
        EXPECT_TRUE(true); // Достигается
    }
}
