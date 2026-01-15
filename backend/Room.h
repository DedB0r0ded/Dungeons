// Room.h
#pragma once


#include "./backend_base.h"
#include "./Entity.h"
#include "./Location.h"
#include "./Inventory.h"
#include "./Enemy.h"
#include "./factories.h"

#include <variant>


namespace dungeons::backend {


    class Room : public Entity {

        size_t room_number_;
        std::weak_ptr<Location> location_;
        int32_t action_counter_;
        std::weak_ptr<Room> previous_room_;
        std::weak_ptr<Room> next_room_;

        static constexpr int32_t DEFAULT_ACTIONS = 3;


    public:
        // Алиасы
        using search_res_t_inner = std::variant<std::monostate, dungeons::backend::Inventory, dungeons::backend::Enemy>;
        using search_res_t = dungeons::Result<std::variant<std::monostate, dungeons::backend::Inventory, dungeons::backend::Enemy>>;
        
        
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
        bool has_actions() const noexcept { return action_counter_ > 0; }


        // Сеттеры
        void room_number(size_t value) noexcept { room_number_ = value; }
        
        void location(std::weak_ptr<Location> value) noexcept { location_ = value; }

        dungeons::Result<void> action_counter(int32_t value) noexcept;

        void reset_actions() noexcept;
        
        void previous_room(std::weak_ptr<Room> value) noexcept { previous_room_ = value; }        
        void next_room(std::weak_ptr<Room> value) noexcept { next_room_ = value; }


        search_res_t nothing_found();
        search_res_t treasure_found(const Location& location_ptr);
        search_res_t enemy_found(const Location& location_ptr, const PlayerAliveComponent& player);
        dungeons::Result<std::variant<std::monostate, Inventory, Enemy>> search(const PlayerAliveComponent& player) noexcept;
     

        // Валидация
        dungeons::Result<void> validate() const noexcept;
        dungeons::Result<void> validate_action_counter() const noexcept;
        dungeons::Result<void> validate_location(const std::shared_ptr<Location> p) const noexcept;
    };


} // namespace dungeons::backend