// Battle.h
#pragma once


#include <cstdint>


namespace dungeons::backend {


    enum class BattleState : uint8_t {
        NONE = 0,
        STARTED,
        CALCULATING,
        PLAYER_WON,
        PLAYER_LOST,
        ONGOING
    };


    struct BattleData {
        BattleState state;
        float power_ratio;
        int64_t player_power;
        int64_t enemy_power;
        float win_probability;


        // Конструкторы
        BattleData() noexcept
            : state(BattleState::NONE),
            power_ratio(0.0f),
            player_power(0),
            enemy_power(0),
            win_probability(0.0f) {
        }

        BattleData(BattleState s, float ratio, int64_t pp, int64_t ep, float wp) noexcept
            : state(s),
            power_ratio(ratio),
            player_power(pp),
            enemy_power(ep),
            win_probability(wp) {
        }


        // Правило пяти
        BattleData(const BattleData&) = default;
        BattleData(BattleData&&) noexcept = default;
        BattleData& operator=(const BattleData&) = default;
        BattleData& operator=(BattleData&&) noexcept = default;
        ~BattleData() = default;
    };


} // namespace dungeons::backend