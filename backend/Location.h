// Location.h
#pragma once

#include "./Entity.h"
#include "../result.h"
#include <string>
#include <memory>

namespace dungeons::backend {


    // Forward declarations for factories
    class EnemyFactory;
    class InventoryFactory;


    class Location : public Entity {
        std::string name_;
        std::string description_;
        std::shared_ptr<EnemyFactory> enemy_factory_;
        std::shared_ptr<InventoryFactory> inventory_factory_;


    public:
        // Конструкторы
        Location() noexcept
            : Entity(),
            name_("Неизвестная локация"),
            description_(""),
            enemy_factory_(nullptr),
            inventory_factory_(nullptr) {
            uid_.set_flag(UidFlags::IS_LOCATION);
        }

        Location(const std::string& name, const std::string& description, uint32_t seed_id) noexcept
            : Entity(seed_id, UidFlags::IS_LOCATION),
            name_(name),
            description_(description),
            enemy_factory_(nullptr),
            inventory_factory_(nullptr) {
        }

        Location(const uid_t& uid, const std::string& name, const std::string& description) noexcept
            : Entity(uid),
            name_(name),
            description_(description),
            enemy_factory_(nullptr),
            inventory_factory_(nullptr) {
        }


        // Правило пяти
        Location(const Location&) = default;
        Location(Location&&) noexcept = default;
        Location& operator=(const Location&) = default;
        Location& operator=(Location&&) noexcept = default;
        ~Location() = default;


        // Геттеры
        const std::string& name() const noexcept { return name_; }
        const std::string& description() const noexcept { return description_; }
        std::shared_ptr<EnemyFactory> enemy_factory() const noexcept { return enemy_factory_; }
        std::shared_ptr<InventoryFactory> inventory_factory() const noexcept { return inventory_factory_; }


        // Сеттеры
        void name(const std::string& value) noexcept { name_ = value; }
        void description(const std::string& value) noexcept { description_ = value; }
        void enemy_factory(std::shared_ptr<EnemyFactory> value) noexcept { enemy_factory_ = value; }
        void inventory_factory(std::shared_ptr<InventoryFactory> value) noexcept { inventory_factory_ = value; }


        // Валидация
        ::dungeons::Result<void> validate() const noexcept {
            if (name_.empty())
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Название локации не может быть пустым");
            if (!uid_.is_location())
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Экземпляр Location должен иметь IS_LOCATION флаг");
            return ::dungeons::Ok();
        }
    };


} // namespace dungeons::backend