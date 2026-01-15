// Player.h
// TODO: refactor Player.completeBattle().
// GUI must be able to track it step by step, not as atomic operation.
// As I want to be able to ask user (player) if he wants to retreat on each step:
// After calculating enemy strength points, 
// After calculating win probability
#pragma once


#include "../backend_base.h"
#include "./PlayerEntityComponent.h"
#include "./PlayerAliveComponent.h"
#include "../Location.h"
#include "../Room.h"
#include "../Inventory.h"
#include "../Enemy.h"
#include "./Battle.h"

#include <variant>
#include <cmath>


namespace dungeons::backend {
    

    class Player {
        PlayerEntityComponent entity_;
        PlayerAliveComponent alive_;
        
        Inventory inventory_;
        std::weak_ptr<Room> current_room_;

        static constexpr float LOSS_THRESHOLD = 0.3f;
        static constexpr float WIN_THRESHOLD = 2.8f;
        static constexpr float WIN_PROBABILITY_MULTIPLIER = 0.35f;
        static constexpr float DAMAGE_REDUCTION = 0.8f;
        static constexpr float LOST_EXPERIENCE_MULTIPLIER = 0.1f;
        static constexpr int32_t LEVEL_UP_ATTRIBUTE_POINTS = 5;


    public:
        // Конструкторы
        Player() : entity_(), alive_(), inventory_(), current_room_() { }

        Player(const std::string& name, uint32_t seed_id) noexcept
            : entity_(seed_id), alive_(name), inventory_(), current_room_() { }

        Player(const uid_t& uid, const std::string& name, int32_t level,
            int32_t base_health, int32_t base_defense, int32_t base_damage,
            const Attributes& base_attrs) noexcept
            : entity_(uid), alive_(name, level, base_health, base_defense, base_damage, base_attrs),
            inventory_(), current_room_() { }


        // Правило пяти
        Player(const Player&) = default;
        Player(Player&&) noexcept = default;
        Player& operator=(const Player&) = default;
        Player& operator=(Player&&) noexcept = default;
        ~Player() = default;


        // Геттеры
        Inventory& inventory() noexcept { return inventory_; }
        const Inventory& inventory() const noexcept { return inventory_; }
        std::weak_ptr<Room> current_room() const noexcept { return current_room_; }


        // Сеттеры
        void current_room(std::weak_ptr<Room> value) noexcept { current_room_ = value; }


        // Передвижение по комнатам
        ::dungeons::Result<void> move_to_next_room() noexcept;
        ::dungeons::Result<void> move_to_previous_room() noexcept;

        // Обыскать текущую комнату
        ::dungeons::Result<std::variant<std::monostate, Inventory, Enemy>> search_room() noexcept;


        // =============Боевая система=============

        BattleData start_battle(const Enemy& enemy) const noexcept;

        // Вычислить вероятность победы
        static float calculate_win_probability(float power_ratio) noexcept {
            return power_ratio * WIN_PROBABILITY_MULTIPLIER;
        }

        // Получить результат битвы (true если игрок победил)
        bool resolve_battle(float win_probability) const noexcept {
            return ::dungeons::Random::next_bool(static_cast<double>(win_probability));
        }


        // Применить изменения после битвы

        ::dungeons::Result<void> apply_battle_damage(const Enemy& enemy, const BattleData& battle, bool player_won) noexcept;
        ::dungeons::Result<void> gain_experience(const BattleData& battle, bool player_won) noexcept;

        // Полный процесс битвы
        ::dungeons::Result<bool> complete_battle(const Enemy& enemy) noexcept;

        // ==========================


        // Валидация
        ::dungeons::Result<void> validate() const noexcept;
    };


} // namespace dungeons::backend