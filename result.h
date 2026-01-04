#pragma once


#include <string>
#include <variant>
#include <utility>


namespace dungeons {
    enum class ErrorCode {
        OK = 0,
        INVALID_ARGUMENT,
        OUT_OF_RANGE,
        EMPTY_CONTAINER,
        VALIDATION_FAILED,
        INVALID_DIMENSIONS
    };


    class Error {
        ErrorCode code_;
        std::string message_;

    public:
        constexpr Error() noexcept : code_(ErrorCode::OK) {}

        Error(ErrorCode code, std::string message)
            : code_(code), message_(std::move(message)) {
        }

        constexpr ErrorCode code() const noexcept { return code_; }
        const std::string& message() const noexcept { return message_; }
        constexpr bool is_ok() const noexcept { return code_ == ErrorCode::OK; }
    };


    template<typename T>
    class Result {
        std::variant<T, Error> data_;

    public:
        // Конструктор успеха
        Result(T value) : data_(std::move(value)) {}

        // Конструктор ошибки
        Result(Error error) : data_(std::move(error)) {}

        Result(ErrorCode code, std::string message)
            : data_(Error(code, std::move(message))) {
        }

        Result(const Result&) = default;
        Result(Result&&) noexcept = default;
        Result& operator=(const Result&) = default;
        Result& operator=(Result&&) noexcept = default;
        ~Result() = default;

        bool is_ok() const noexcept {
            return std::holds_alternative<T>(data_);
        }

        bool is_error() const noexcept {
            return std::holds_alternative<Error>(data_);
        }

        const T& value() const& {
            return std::get<T>(data_);
        }

        T& value()& {
            return std::get<T>(data_);
        }

        T&& value()&& {
            return std::get<T>(std::move(data_));
        }

        const Error& error() const& {
            return std::get<Error>(data_);
        }

        Error& error()& {
            return std::get<Error>(data_);
        }

        // Удобный оператор для проверки успеха
        explicit operator bool() const noexcept {
            return is_ok();
        }
    };


    // Специализация для void
    template<>
    class Result<void> {
        Error error_;

    public:
        Result() noexcept : error_() {}

        Result(Error error) : error_(std::move(error)) {}

        Result(ErrorCode code, std::string message)
            : error_(code, std::move(message)) {
        }

        bool is_ok() const noexcept {
            return error_.is_ok();
        }

        bool is_error() const noexcept {
            return !error_.is_ok();
        }

        const Error& error() const noexcept {
            return error_;
        }

        explicit operator bool() const noexcept {
            return is_ok();
        }
    };


    // Вспомогательные функции для создания Result
    template<typename T>
    Result<T> Ok(T value) {
        return Result<T>(std::move(value));
    }



    inline Result<void> Ok() {
        return Result<void>();
    }

    template<typename T>
    Result<T> Err(ErrorCode code, std::string message) {
        return Result<T>(Error(code, std::move(message)));
    }

    inline Result<void> Err(ErrorCode code, std::string message) {
        return Result<void>(Error(code, std::move(message)));
    }
}