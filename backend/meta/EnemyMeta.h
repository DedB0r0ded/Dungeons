// EnemyMeta.h
#pragma once


#include "../Entity.h"


namespace dungeons::backend {
    class EnemyMeta : public Entity {
        std::string name_;

        static constexpr float GENERATION_BASIS = 1.0f;


    public:
        // Constructors
        EnemyMeta() noexcept : Entity(), name_("Неизвестный протвник") {
            uid_.set_flag(UidFlags::IS_ENEMY);
        }

        EnemyMeta(const std::string& name, uint32_t seed_id) noexcept
            : Entity(seed_id, UidFlags::IS_ENEMY),
            name_(name) {
        }

        explicit EnemyMeta(const uid_t& uid, const std::string& name) noexcept
            : Entity(uid),
            name_(name) {
        }


        // Правило пяти
        EnemyMeta(const EnemyMeta&) = default;
        EnemyMeta(EnemyMeta&&) noexcept = default;
        EnemyMeta& operator=(const EnemyMeta&) = default;
        EnemyMeta& operator=(EnemyMeta&&) noexcept = default;
        ~EnemyMeta() = default;


        // Геттеры
        const std::string& name() const noexcept { return name_; }
        static constexpr float generation_basis() noexcept { return GENERATION_BASIS; }


        // Сеттеры
        void name(const std::string& value) noexcept { name_ = value; }


        // Валидация
        ::dungeons::Result<void> validate() const noexcept {
            if (name_.empty())
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Имя не может быть пустым");
            if (!uid_.is_enemy())
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Экземпляр EnemyMeta должен иметь IS_ENEMY флаг");
            return ::dungeons::Ok();
        }
    };
}