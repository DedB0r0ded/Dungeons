// Room.h
#pragma once


#include "./Entity.h"
#include "./Location.h"
#include "./Inventory.h"
#include "./Enemy.h"
#include "./backend_base.h"

#include <variant>
#include <memory>


namespace dungeons::backend {


    class Room : public Entity {
        size_t room_number_;
        std::weak_ptr<Location> location_;
        int32_t action_counter_;
        std::weak_ptr<Room> previous_room_;
        std::weak_ptr<Room> next_room_;

        static constexpr int32_t DEFAULT_ACTIONS = 3;


    public:
        // Конструкторы
        Room() noexcept
            : Entity(),
            room_number_(0),
            location_(),
            action_counter_(DEFAULT_ACTIONS),
            previous_room_(),
            next_room_() {
            uid_.set_flag(UidFlags::IS_ROOM);
        }

        Room(size_t room_number, std::weak_ptr<Location> location, uint32_t seed_id) noexcept
            : Entity(seed_id, UidFlags::IS_ROOM),
            room_number_(room_number),
            location_(location),
            action_counter_(DEFAULT_ACTIONS),
            previous_room_(),
            next_room_() {
        }

        Room(const uid_t& uid, size_t room_number, std::weak_ptr<Location> location,
            int32_t action_counter) noexcept
            : Entity(uid),
            room_number_(room_number),
            location_(location),
            action_counter_(action_counter),
            previous_room_(),
            next_room_() {
        }


        // Правило пяти
        Room(const Room&) = default;
        Room(Room&&) noexcept = default;
        Room& operator=(const Room&) = default;
        Room& operator=(Room&&) noexcept = default;
        ~Room() = default;


        // Геттеры
        size_t room_number() const noexcept { return room_number_; }
        std::weak_ptr<Location> location() const noexcept { return location_; }
        int32_t action_counter() const noexcept { return action_counter_; }
        std::weak_ptr<Room> previous_room() const noexcept { return previous_room_; }
        std::weak_ptr<Room> next_room() const noexcept { return next_room_; }


        // Сеттеры
        void room_number(size_t value) noexcept { room_number_ = value; }
        void location(std::weak_ptr<Location> value) noexcept { location_ = value; }

        ::dungeons::Result<void> action_counter(int32_t value) noexcept {
            if (value < 0)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Счетчик действий не может быть отрицательным");
            action_counter_ = value;
            return ::dungeons::Ok();
        }

        void previous_room(std::weak_ptr<Room> value) noexcept { previous_room_ = value; }
        void next_room(std::weak_ptr<Room> value) noexcept { next_room_ = value; }


        bool has_actions() const noexcept {
            return action_counter_ > 0;
        }

        // Обыскать комнату
        // Возвращает: monostate (ничего), Inventory (сокровище) или Enemy
        ::dungeons::Result<std::variant<std::monostate, Inventory, Enemy>> search() noexcept {
            if (action_counter_ <= 0)
                return ::dungeons::Err<std::variant<std::monostate, Inventory, Enemy>>(
                    ::dungeons::ErrorCode::VALIDATION_FAILED, "Счетчик действий равен нулю");
            auto location_ptr = location_.lock();
            if (!location_ptr)
                return ::dungeons::Err<std::variant<std::monostate, Inventory, Enemy>>(
                    ::dungeons::ErrorCode::INVALID_ARGUMENT, "Комната не связана с локацией");
            action_counter_--;
            // Определение результата: 0 = ничего, 1 = сокровище, 2 = враг
            // Вероятности: 40% ничего, 30% сокровище, 30% враг
            float roll = ::dungeons::Random::next_float(0.0f, 1.0f);
            if (roll < 0.4f) {
                return ::dungeons::Ok(std::variant<std::monostate, Inventory, Enemy>(std::monostate{}));
            }
            else if (roll < 0.7f) {
                auto inventory_factory = location_ptr->inventory_factory();
                if (!inventory_factory)
                    return ::dungeons::Ok(std::variant<std::monostate, Inventory, Enemy>(std::monostate{}));
                // Create treasure inventory using factory
                // Note: This requires InventoryFactory to be implemented
                // For now, return empty inventory as placeholder
                Inventory treasure;
                return ::dungeons::Ok(std::variant<std::monostate, Inventory, Enemy>(treasure));
            }
            else {
                auto enemy_factory = location_ptr->enemy_factory();
                if (!enemy_factory)
                    return ::dungeons::Ok(std::variant<std::monostate, Inventory, Enemy>(std::monostate{}));
                // Create enemy using factory
                // Note: This requires EnemyFactory to be implemented
                // For now, return default enemy as placeholder
                Enemy enemy;
                return ::dungeons::Ok(std::variant<std::monostate, Inventory, Enemy>(enemy));
            }
        }


        // Сбросить счетчик действий
        void reset_actions() noexcept {
            action_counter_ = DEFAULT_ACTIONS;
        }


        // Валидация
        ::dungeons::Result<void> validate() const noexcept {
            if (!uid_.is_room())
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Room must have IS_ROOM flag");
            if (action_counter_ < 0)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Action counter cannot be negative");
            return ::dungeons::Ok();
        }
    };


} // namespace dungeons::backend