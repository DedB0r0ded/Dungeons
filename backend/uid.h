// uid.h
#pragma once

#include <cstdint>
#include <cstring>
#include "../time.h"
#include "../result.h"
#include "../Random.h"

namespace dungeons::backend {


    enum class UidFlags : uint32_t {
        NONE = 0,
        IS_PLAYER = 1 << 0,
        IS_ENEMY = 1 << 1,
        IS_WEAPON = 1 << 2,
        IS_ARMOR = 1 << 3,
        IS_INVENTORY = 1 << 4,
        IS_ITEM = 1 << 5,
        IS_LOCATION = 1 << 6,
        IS_ROOM = 1 << 7,
    };

    // Операторы для работы с флагами
    inline UidFlags operator|(UidFlags a, UidFlags b) noexcept {
        return static_cast<UidFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline UidFlags operator&(UidFlags a, UidFlags b) noexcept {
        return static_cast<UidFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline UidFlags operator^(UidFlags a, UidFlags b) noexcept {
        return static_cast<UidFlags>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
    }

    inline UidFlags operator~(UidFlags a) noexcept {
        return static_cast<UidFlags>(~static_cast<uint32_t>(a));
    }

    inline UidFlags& operator|=(UidFlags& a, UidFlags b) noexcept {
        a = a | b;
        return a;
    }

    inline UidFlags& operator&=(UidFlags& a, UidFlags b) noexcept {
        a = a & b;
        return a;
    }

    inline UidFlags& operator^=(UidFlags& a, UidFlags b) noexcept {
        a = a ^ b;
        return a;
    }

    inline bool has_flag(UidFlags flags, UidFlags flag) noexcept {
        return (static_cast<uint32_t>(flags) & static_cast<uint32_t>(flag)) != 0;
    }


    class uid_t {
    private:
        uint32_t seed_id_;
        UidFlags flags_;
        ::dungeons::Time timestamp_;
        uint64_t random_id_;


    public:
        // Конструкторы
        uid_t() noexcept
            : seed_id_(0),
            flags_(UidFlags::NONE),
            timestamp_(),
            random_id_(0) {
        }

        uid_t(uint32_t seed_id, UidFlags flags, const ::dungeons::Time& timestamp, uint64_t random_id) noexcept
            : seed_id_(seed_id),
            flags_(flags),
            timestamp_(timestamp),
            random_id_(random_id) {
        }


        // Статический метод для генерации нового UID
        static uid_t generate(uint32_t seed_id, UidFlags flags) noexcept {
            auto timestamp_result = ::dungeons::Time::now();
            ::dungeons::Time timestamp = timestamp_result ? timestamp_result.value() : ::dungeons::Time();
            uint64_t random_id = ::dungeons::Random::next_ulong_long();
            return uid_t(seed_id, flags, timestamp, random_id);
        }

        // Статический метод для создания из репозитория (с существующим timestamp)
        static uid_t from_repository(uint32_t seed_id, UidFlags flags,
            const ::dungeons::Time& timestamp,
            uint64_t random_id) noexcept {
            return uid_t(seed_id, flags, timestamp, random_id);
        }


        // Правило пяти
        uid_t(const uid_t&) = default;
        uid_t(uid_t&&) noexcept = default;
        uid_t& operator=(const uid_t&) = default;
        uid_t& operator=(uid_t&&) noexcept = default;
        ~uid_t() = default;


        // Геттеры
        uint32_t seed_id() const noexcept { return seed_id_; }
        UidFlags flags() const noexcept { return flags_; }
        ::dungeons::Time timestamp() const noexcept { return timestamp_; }
        uint64_t random_id() const noexcept { return random_id_; }


        // Сеттеры
        void seed_id(uint32_t value) noexcept { seed_id_ = value; }
        void flags(UidFlags value) noexcept { flags_ = value; }
        void timestamp(const ::dungeons::Time& value) noexcept { timestamp_ = value; }
        void random_id(uint64_t value) noexcept { random_id_ = value; }


        // Работа с флагами
        bool has_flag(UidFlags flag) const noexcept {
            return ::dungeons::backend::has_flag(flags_, flag);
        }

        void set_flag(UidFlags flag) noexcept {
            flags_ |= flag;
        }

        void clear_flag(UidFlags flag) noexcept {
            flags_ &= ~flag;
        }

        void toggle_flag(UidFlags flag) noexcept {
            flags_ ^= flag;
        }


        // Булевые геттеры для конкретных флагов
        bool is_player() const noexcept { return has_flag(UidFlags::IS_PLAYER); }
        bool is_enemy() const noexcept { return has_flag(UidFlags::IS_ENEMY); }
        bool is_weapon() const noexcept { return has_flag(UidFlags::IS_WEAPON); }
        bool is_armor() const noexcept { return has_flag(UidFlags::IS_ARMOR); }
        bool is_inventory() const noexcept { return has_flag(UidFlags::IS_INVENTORY); }
        bool is_item() const noexcept { return has_flag(UidFlags::IS_ITEM); }
        bool is_location() const noexcept { return has_flag(UidFlags::IS_LOCATION); }
        bool is_room() const noexcept { return has_flag(UidFlags::IS_ROOM); }


        // Булевые сеттеры для конкретных флагов
        void is_player(bool value) noexcept {
            if (value) set_flag(UidFlags::IS_PLAYER);
            else clear_flag(UidFlags::IS_PLAYER);
        }

        void is_enemy(bool value) noexcept {
            if (value) set_flag(UidFlags::IS_ENEMY);
            else clear_flag(UidFlags::IS_ENEMY);
        }

        void is_weapon(bool value) noexcept {
            if (value) set_flag(UidFlags::IS_WEAPON);
            else clear_flag(UidFlags::IS_WEAPON);
        }

        void is_armor(bool value) noexcept {
            if (value) set_flag(UidFlags::IS_ARMOR);
            else clear_flag(UidFlags::IS_ARMOR);
        }

        void is_inventory(bool value) noexcept {
            if (value) set_flag(UidFlags::IS_INVENTORY);
            else clear_flag(UidFlags::IS_INVENTORY);
        }

        void is_item(bool value) noexcept {
            if (value) set_flag(UidFlags::IS_ITEM);
            else clear_flag(UidFlags::IS_ITEM);
        }

        void is_location(bool value) noexcept {
            if (value) set_flag(UidFlags::IS_LOCATION);
            else clear_flag(UidFlags::IS_LOCATION);
        }

        void is_room(bool value) noexcept {
            if (value) set_flag(UidFlags::IS_ROOM);
            else clear_flag(UidFlags::IS_ROOM);
        }


        // Операторы сравнения
        bool operator==(const uid_t& other) const noexcept {
            return seed_id_ == other.seed_id_ &&
                flags_ == other.flags_ &&
                timestamp_ == other.timestamp_ &&
                random_id_ == other.random_id_;
        }

        bool operator!=(const uid_t& other) const noexcept {
            return !(*this == other);
        }


        // Сериализация
        void to_bytes(uint8_t* buffer) const noexcept {
            std::memcpy(buffer, this, sizeof(uid_t));
        }

        static ::dungeons::Result<uid_t> from_bytes(const uint8_t* buffer) {
            if (buffer == nullptr)
                return ::dungeons::Err<uid_t>(::dungeons::ErrorCode::INVALID_ARGUMENT, "Buffer pointer is null");
            uid_t uid;
            std::memcpy(&uid, buffer, sizeof(uid_t));
            return ::dungeons::Ok(uid);
        }
    };


} // namespace dungeons::backend