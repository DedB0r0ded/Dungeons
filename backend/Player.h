// Player.h
// TODO: refactor Player.completeBattle().
// GUI must be able to track it step by step, not as atomic operation.
// As I want to be able to ask user (player) if he wants to retreat on each step:
// After calculating enemy strength points, 
// After calculating win probability
#pragma once


#include "./Entity.h"
#include "./Alive.h"
#include "./Inventory.h"
#include "./Room.h"
#include "./Enemy.h"
#include "./Battle.h"
#include "./backend_base.h"

#include <memory>
#include <variant>
#include <cmath>


namespace dungeons::backend {
    

    class Player : public Entity, public Alive {
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
        Player() noexcept
            : Entity(),
            Alive(),
            inventory_(),
            current_room_() {
            uid_.set_flag(UidFlags::IS_PLAYER);
        }

        Player(const std::string& name, uint32_t seed_id) noexcept
            : Entity(seed_id, UidFlags::IS_PLAYER),
            Alive(name, 1, 100, 10, 10, Attributes(100, 10)),
            inventory_(),
            current_room_() {
        }

        Player(const uid_t& uid, const std::string& name, int32_t level,
            int32_t base_health, int32_t base_defense, int32_t base_damage,
            const Attributes& base_attrs) noexcept
            : Entity(uid),
            Alive(name, level, base_health, base_defense, base_damage, base_attrs),
            inventory_(),
            current_room_() {
        }


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
        ::dungeons::Result<void> move_to_next_room() noexcept {
            auto room_ptr = current_room_.lock();
            if (!room_ptr)
                return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT, "Игрок не в комнате");
            auto next_room = room_ptr->next_room().lock();
            if (!next_room)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Следующей комнаты не существует");
            current_room_ = next_room;
            return ::dungeons::Ok();
        }

        ::dungeons::Result<void> move_to_previous_room() noexcept {
            auto room_ptr = current_room_.lock();
            if (!room_ptr)
                return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT, "Игрок не в комнате");
            auto prev_room = room_ptr->previous_room().lock();
            if (!prev_room)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Предыдущей комнаты не существует");
            current_room_ = prev_room;
            return ::dungeons::Ok();
        }

        // Обыскать текущую комнату
        ::dungeons::Result<std::variant<std::monostate, Inventory, Enemy>> search_room() noexcept {
            auto room_ptr = current_room_.lock();
            if (!room_ptr)
                return ::dungeons::Err<std::variant<std::monostate, Inventory, Enemy>>(
                    ::dungeons::ErrorCode::INVALID_ARGUMENT, "Игрок не в комнате");
            return room_ptr->search();
        }


        // Боевая система
        BattleData start_battle(const Enemy& enemy) const noexcept {
            // Высчитываем очки силы
            int64_t player_power = calculate_power_score();
            int64_t enemy_power = enemy.calculate_power_score();
            // Посчитать коэффициент уровня
            float level_coefficient = static_cast<float>(level_) / static_cast<float>(enemy.level());
            // Применить коэффициент
            player_power = static_cast<int64_t>(player_power * level_coefficient);
            enemy_power = static_cast<int64_t>(enemy_power / level_coefficient);
            // Вычислить соотношение сил
            float power_ratio = 0.0f;
            if (enemy_power > 0) {
                power_ratio = static_cast<float>(player_power) / static_cast<float>(enemy_power);
            }
            else {
                power_ratio = 9999.999;
            }
            // Определить состояние битвы
            BattleState state;
            if (power_ratio < LOSS_THRESHOLD) {
                state = BattleState::PLAYER_LOST;
            }
            else if (power_ratio > WIN_THRESHOLD) {
                state = BattleState::PLAYER_WON;
            }
            else {
                state = BattleState::ONGOING;
            }
            // Вычислить вероятность победы
            float win_probability = calculate_win_probability(power_ratio);
            return BattleData(state, power_ratio, player_power, enemy_power, win_probability);
        }

        // Вычислить вероятность победы
        float calculate_win_probability(float power_ratio) const noexcept {
            return power_ratio * WIN_PROBABILITY_MULTIPLIER;
        }

        // Получить результат битвы (true если игрок победил)
        bool resolve_battle(float win_probability) const noexcept {
            return ::dungeons::Random::next_bool(static_cast<double>(win_probability));
        }

        // Применить урон после битвы
        ::dungeons::Result<void> apply_battle_damage(const Enemy& enemy, const BattleData& battle,
            bool player_won) noexcept {
            int32_t damage = 0;
            if (player_won) {
                // При победе: damage = (enemy_attack - player_defense) / power_ratio * 0.8
                int32_t enemy_attack = enemy.calculate_attack();
                int32_t player_defense = calculate_defense();
                int32_t base_damage = enemy_attack - player_defense;
                if (base_damage > 0 && battle.power_ratio > 0) {
                    damage = static_cast<int32_t>(
                        (base_damage / battle.power_ratio) * DAMAGE_REDUCTION);
                }
            }
            else {
                // При поражении: damage = enemy_attack / power_ratio
                int32_t enemy_attack = enemy.calculate_attack();
                if (battle.power_ratio > 0) {
                    damage = static_cast<int32_t>(enemy_attack / battle.power_ratio);
                }
                else {
                    damage = enemy_attack;
                }
            }
            return take_damage(damage);
        }

        // Получение опыта из битвы
        ::dungeons::Result<void> gain_experience(const BattleData& battle, bool player_won) noexcept {
            int64_t exp_gained = 0;
            if (player_won) {
                // При победе забираем всю силу противника как опыт
                exp_gained = battle.enemy_power;
            }
            else {
                // При поражении забираем 10% силы противника как опыт
                exp_gained = static_cast<int64_t>(battle.enemy_power * LOST_EXPERIENCE_MULTIPLIER);
            }
            auto add_result = add_experience(exp_gained);
            if (!add_result)
                return add_result;
            // Проверка на поднятие уровня
            while (can_level_up()) {
                auto level_up_result = level_up(LEVEL_UP_ATTRIBUTE_POINTS);
                if (!level_up_result)
                    return ::dungeons::Err(level_up_result.error().code(), level_up_result.error().message());
                // Очки атрибутов получены, но не распределены
                // Это будет реализовано в пределах слоя игровой логики
            }
            return ::dungeons::Ok();
        }

        // Полный процесс битвы
        ::dungeons::Result<bool> complete_battle(const Enemy& enemy) noexcept {
            // Начать битву
            BattleData battle = start_battle(enemy);
            // Обработка быстрых результатов
            if (battle.state == BattleState::PLAYER_LOST) {
                auto damage_result = apply_battle_damage(enemy, battle, false);
                if (!damage_result) 
                    return ::dungeons::Err<bool>(damage_result.error().code(), damage_result.error().message());
                auto exp_result = gain_experience(battle, false);
                if (!exp_result) 
                    return ::dungeons::Err<bool>(exp_result.error().code(), exp_result.error().message());
                return ::dungeons::Ok(false);
            }
            if (battle.state == BattleState::PLAYER_WON) {
                auto damage_result = apply_battle_damage(enemy, battle, true);
                if (!damage_result) 
                    return ::dungeons::Err<bool>(damage_result.error().code(), damage_result.error().message());
                auto exp_result = gain_experience(battle, true);
                if (!exp_result) 
                    return ::dungeons::Err<bool>(exp_result.error().code(), exp_result.error().message());
                return ::dungeons::Ok(true);
            }
            // Продолжение битвы - решить через вероятность
            bool won = resolve_battle(battle.win_probability);
            auto damage_result = apply_battle_damage(enemy, battle, won);
            if (!damage_result)
                return ::dungeons::Err<bool>(damage_result.error().code(), damage_result.error().message());
            auto exp_result = gain_experience(battle, won);
            if (!exp_result) 
                return ::dungeons::Err<bool>(exp_result.error().code(), exp_result.error().message());
            return ::dungeons::Ok(won);
        }


        // Валидация
        ::dungeons::Result<void> validate() const noexcept {
            if (!uid_.is_player())
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Экземпляр Player должен иметь IS_PLAYER флаг");
            auto alive_validation = Alive::validate();
            if (!alive_validation)
                return alive_validation;
            auto inventory_validation = inventory_.validate();
            if (!inventory_validation)
                return inventory_validation;
            return ::dungeons::Ok();
        }
    };


} // namespace dungeons::backend