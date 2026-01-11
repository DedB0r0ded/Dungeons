// Attributes.h
#pragma once

#include <cstdint>

namespace dungeons::backend {


    struct Attributes {
        int32_t health;
        int32_t strength;


        // Конструкторы
        Attributes() noexcept : health(0), strength(0) {}

        Attributes(int32_t h, int32_t s) noexcept : health(h), strength(s) {}


        // Правило пяти
        Attributes(const Attributes&) = default;
        Attributes(Attributes&&) noexcept = default;
        Attributes& operator=(const Attributes&) = default;
        Attributes& operator=(Attributes&&) noexcept = default;
        ~Attributes() = default;


        // Операторы сравнения
        bool operator==(const Attributes& other) const noexcept {
            return health == other.health && strength == other.strength;
        }

        bool operator!=(const Attributes& other) const noexcept {
            return !(*this == other);
        }


        // Арифметические операторы
        Attributes operator+(const Attributes& other) const noexcept {
            return Attributes(health + other.health, strength + other.strength);
        }

        Attributes operator-(const Attributes& other) const noexcept {
            return Attributes(health - other.health, strength - other.strength);
        }

        Attributes& operator+=(const Attributes& other) noexcept {
            health += other.health;
            strength += other.strength;
            return *this;
        }

        Attributes& operator-=(const Attributes& other) noexcept {
            health -= other.health;
            strength -= other.strength;
            return *this;
        }


        // Умножение на скаляр
        Attributes operator*(float multiplier) const noexcept {
            return Attributes(
                static_cast<int32_t>(health * multiplier),
                static_cast<int32_t>(strength * multiplier)
            );
        }

        Attributes& operator*=(float multiplier) noexcept {
            health = static_cast<int32_t>(health * multiplier);
            strength = static_cast<int32_t>(strength * multiplier);
            return *this;
        }
    };


} // namespace dungeons::backend