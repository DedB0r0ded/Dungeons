// Money.h
#pragma once


#include "backend_base.h"


namespace dungeons::backend {


    class Money {
        int64_t amount_;


    public:
        // Конструкторы
        Money() noexcept : amount_(0) {}

        explicit Money(int64_t amount) noexcept : amount_(amount) {}


        // Правило пяти
        Money(const Money&) = default;
        Money(Money&&) noexcept = default;
        Money& operator=(const Money&) = default;
        Money& operator=(Money&&) noexcept = default;
        ~Money() = default;


        // Геттер/сеттер
        int64_t amount() const noexcept { return amount_; }

        ::dungeons::Result<void> amount(int64_t value) noexcept {
            if (value < 0) {
                return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT,
                    "Money amount cannot be negative");
            }
            amount_ = value;
            return ::dungeons::Ok();
        }


        // Валидация
        ::dungeons::Result<void> validate() const noexcept {
            if (amount_ < 0) {
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED,
                    "Money amount is negative");
            }
            return ::dungeons::Ok();
        }


        // Операции с деньгами
        ::dungeons::Result<void> add(int64_t value) noexcept {
            if (value < 0) {
                return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT,
                    "Cannot add negative amount");
            }
            amount_ += value;
            return ::dungeons::Ok();
        }

        ::dungeons::Result<void> subtract(int64_t value) noexcept {
            if (value < 0)
                return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT, "Cannot subtract negative amount");
            if (amount_ < value)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Insufficient funds");
            amount_ -= value;
            return ::dungeons::Ok();
        }

        ::dungeons::Result<void> add(const Money& other) noexcept {
            return add(other.amount_);
        }

        ::dungeons::Result<void> subtract(const Money& other) noexcept {
            return subtract(other.amount_);
        }


        // Операторы сравнения
        bool operator==(const Money& other) const noexcept {
            return amount_ == other.amount_;
        }

        bool operator!=(const Money& other) const noexcept {
            return amount_ != other.amount_;
        }

        bool operator<(const Money& other) const noexcept {
            return amount_ < other.amount_;
        }

        bool operator<=(const Money& other) const noexcept {
            return amount_ <= other.amount_;
        }

        bool operator>(const Money& other) const noexcept {
            return amount_ > other.amount_;
        }

        bool operator>=(const Money& other) const noexcept {
            return amount_ >= other.amount_;
        }


        // Арифметические операторы
        Money operator+(const Money& other) const noexcept {
            return Money(amount_ + other.amount_);
        }

        Money operator-(const Money& other) const noexcept {
            return Money(amount_ - other.amount_);
        }

        Money& operator+=(const Money& other) noexcept {
            amount_ += other.amount_;
            return *this;
        }

        Money& operator-=(const Money& other) noexcept {
            amount_ -= other.amount_;
            return *this;
        }
    };


} // namespace dungeons::backend