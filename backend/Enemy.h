// Enemy.h
#pragma once


#include "./Entity.h"
#include "./Alive.h"
#include "./Inventory.h"
#include "./backend_base.h"

#include <string>
#include <memory>


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


    class Enemy : public Alive {
        BaseInventory inventory_;
        std::weak_ptr<EnemyMeta> meta_;


    public:
        // Конструкторы
        Enemy() noexcept
            : Alive(),
            inventory_(),
            meta_() {
        }

        Enemy(const std::string& name, int32_t level,
            int32_t base_health, int32_t base_defense, int32_t base_damage,
            const Attributes& base_attrs, std::weak_ptr<EnemyMeta> meta) noexcept
            : Alive(name, level, base_health, base_defense, base_damage, base_attrs),
            inventory_(),
            meta_(meta) {
        }

        Enemy(const std::string& name, int32_t level,
            int32_t base_health, int32_t base_defense, int32_t base_damage,
            const Attributes& base_attrs, size_t inventory_size,
            std::weak_ptr<EnemyMeta> meta) noexcept
            : Alive(name, level, base_health, base_defense, base_damage, base_attrs),
            inventory_(inventory_size),
            meta_(meta) {
        }


        // Правило пяти
        Enemy(const Enemy&) = default;
        Enemy(Enemy&&) noexcept = default;
        Enemy& operator=(const Enemy&) = default;
        Enemy& operator=(Enemy&&) noexcept = default;
        ~Enemy() = default;


        // Геттеры
        BaseInventory& inventory() noexcept { return inventory_; }
        const BaseInventory& inventory() const noexcept { return inventory_; }
        std::weak_ptr<EnemyMeta> meta() const noexcept { return meta_; }


        // Сеттеры
        void meta(std::weak_ptr<EnemyMeta> value) noexcept { meta_ = value; }


        // Валидация
        ::dungeons::Result<void> validate() const noexcept {
            auto alive_validation = Alive::validate();
            if (!alive_validation)
                return alive_validation;
            auto inventory_validation = inventory_.validate();
            if (!inventory_validation)
                return inventory_validation;
            return ::dungeons::Ok();
        }


        // Забрать добычу (возвращает инвентарь)
        BaseInventory get_loot() noexcept {
            BaseInventory loot = inventory_;
            inventory_.clear();
            return loot;
        }
    };


} // namespace dungeons::backend