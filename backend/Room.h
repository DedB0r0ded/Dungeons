// Room.h
#pragma once


#include "./Entity.h"
#include "./Location.h"
#include "./Inventory.h"
#include "./Enemy.h"
#include "./backend_base.h"
#include "../Logger.h"

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

        bool has_actions() const noexcept {
            return action_counter_ > 0;
        }

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


        // Обыскать комнату (требует передачи игрока для генерации врагов)
        // Возвращает: monostate (ничего), Inventory (сокровище) или Enemy
        ::dungeons::Result<std::variant<std::monostate, Inventory, Enemy>> search(const class Player& player) noexcept {
            if (action_counter_ <= 0) {
                ::dungeons::Logger::instance().warning("Попытка обыскать комнату без действий");
                return ::dungeons::Err<std::variant<std::monostate, Inventory, Enemy>>(
                    ::dungeons::ErrorCode::VALIDATION_FAILED, "Счетчик действий равен нулю");
            }
            auto location_ptr = location_.lock();
            if (!location_ptr) {
                ::dungeons::Logger::instance().error("Комната не связана с локацией");
                return ::dungeons::Err<std::variant<std::monostate, Inventory, Enemy>>(
                    ::dungeons::ErrorCode::INVALID_ARGUMENT, "Комната не связана с локацией");
            }
            action_counter_--;
            ::dungeons::Logger::instance().info(
                "Обыск комнаты #" + std::to_string(room_number_) +
                ", осталось действий: " + std::to_string(action_counter_));
            // Определение результата: 0 = ничего, 1 = сокровище, 2 = враг
            // Вероятности: 40% ничего, 30% сокровище, 30% враг
            float roll = ::dungeons::Random::next_float(0.0f, 1.0f);
            if (roll < 0.4f) {
                ::dungeons::Logger::instance().info("Обыск комнаты: ничего не найдено");
                return ::dungeons::Ok(std::variant<std::monostate, Inventory, Enemy>(std::monostate{}));
            }
            else if (roll < 0.7f) {
                // Найдено сокровище
                auto inventory_factory = location_ptr->inventory_factory();
                if (!inventory_factory) {
                    ::dungeons::Logger::instance().warning("InventoryFactory отсутствует, возвращаем пустой результат");
                    return ::dungeons::Ok(std::variant<std::monostate, Inventory, Enemy>(std::monostate{}));
                }
                Inventory treasure = inventory_factory->create_treasure(room_number_);
                ::dungeons::Logger::instance().info(
                    "Обыск комнаты: найдено сокровище с " +
                    std::to_string(treasure.size()) + " предметами");
                return ::dungeons::Ok(std::variant<std::monostate, Inventory, Enemy>(treasure));
            }
            else {
                // Найден враг
                auto enemy_factory = location_ptr->enemy_factory();
                if (!enemy_factory) {
                    ::dungeons::Logger::instance().warning("EnemyFactory отсутствует, возвращаем пустой результат");
                    return ::dungeons::Ok(std::variant<std::monostate, Inventory, Enemy>(std::monostate{}));
                }
                auto enemy_result = enemy_factory->create_for_player(player);
                if (!enemy_result) {
                    ::dungeons::Logger::instance().error("Ошибка создания врага: " + enemy_result.error().message());
                    return ::dungeons::Err<std::variant<std::monostate, Inventory, Enemy>>(
                        enemy_result.error().code(), enemy_result.error().message());
                }
                ::dungeons::Logger::instance().info("Обыск комнаты: найден противник '" + enemy_result.value().name() + "'");
                return ::dungeons::Ok(std::variant<std::monostate, Inventory, Enemy>(enemy_result.value()));
            }
        }

        // Сбросить счетчик действий
        void reset_actions() noexcept {
            action_counter_ = DEFAULT_ACTIONS;
            ::dungeons::Logger::instance().debug("Сброс счетчика действий комнаты #" + std::to_string(room_number_));
        }


        // Валидация
        ::dungeons::Result<void> validate() const noexcept {
            if (!uid_.is_room())
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Экземпляр Room должен иметь IS_ROOM флаг");
            if (action_counter_ < 0)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Счетчик действий не может быть отрицательным");
            return ::dungeons::Ok();
        }
    };


} // namespace dungeons::backend