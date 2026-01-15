// Enemy.h
#pragma once


#include "./backend_base.h"
#include "./Entity.h"
#include "./Alive.h"
#include "./Inventory.h"
#include "./meta/EnemyMeta.h"

#include <string>
#include <memory>


namespace dungeons::backend {


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