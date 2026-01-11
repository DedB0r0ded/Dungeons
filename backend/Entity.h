// Entity.h
#pragma once


#include "./uid.h"
#include "../result.h"


namespace dungeons::backend {


    // Forward declaration for SQLiteSerializable
    class SQLiteSerializable {
    public:
        virtual ~SQLiteSerializable() = default;


        // Методы для сериализации/десериализации
        // Они должны быть переопределены в дочерних классах
        virtual ::dungeons::Result<std::string> serialize() const = 0;
        virtual ::dungeons::Result<void> deserialize(const std::string& data) = 0;
    };

    class Entity : public SQLiteSerializable {
    protected:
        uid_t uid_;


    public:
        // Конструкторы
        Entity() noexcept : uid_() {}

        explicit Entity(const uid_t& uid) noexcept : uid_(uid) {}

        Entity(uint32_t seed_id, UidFlags flags) noexcept
            : uid_(uid_t::generate(seed_id, flags)) {
        }


        // Правило пяти
        Entity(const Entity&) = default;
        Entity(Entity&&) noexcept = default;
        Entity& operator=(const Entity&) = default;
        Entity& operator=(Entity&&) noexcept = default;
        virtual ~Entity() = default;


        // Геттер/сеттер для uid
        const uid_t& uid() const noexcept { return uid_; }
        void uid(const uid_t& value) noexcept { uid_ = value; }


        // SQLiteSerializable - базовая реализация
        virtual ::dungeons::Result<std::string> serialize() const override {
            return ::dungeons::Err<std::string>(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "Сериализация не реализована для класса Entity");
        }

        virtual ::dungeons::Result<void> deserialize(const std::string& data) override {
            return ::dungeons::Err(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "Десериализация не реализована для класса Entity");
        }
    };


} // namespace dungeons::backend