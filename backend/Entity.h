// Entity.h
#pragma once


#include "./uid.h"
#include "../result.h"


namespace dungeons::backend {


    class Entity {
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
    };


} // namespace dungeons::backend