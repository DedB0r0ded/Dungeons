// Alive.h
// TODO: translate to russian
#pragma once


#include "./Attributes.h"
#include "./Item.h"
#include "../result.h"

#include <string>
#include <memory>
#include <cmath>


namespace dungeons::backend {


    class Alive {
    protected:
        std::string name_;
        int32_t level_;
        int64_t experience_;
        int64_t required_experience_;
        int32_t base_max_health_;
        int32_t max_health_;
        int32_t current_health_;
        int32_t base_defense_;
        int32_t base_damage_;
        Attributes base_attributes_;
        std::shared_ptr<Weapon> active_weapon_;
        std::shared_ptr<Armor> active_armor_;

        static constexpr float EXPERIENCE_MULTIPLIER = 0.05f;
        static constexpr int64_t EXPERIENCE_BASE = 100;


        // Calculate required experience for level
        int64_t calculate_required_experience(int32_t level) const noexcept {
            return static_cast<int64_t>(level * EXPERIENCE_MULTIPLIER * EXPERIENCE_BASE);
        }


    public:
        // Constructors
        Alive() noexcept
            : name_("Unknown"),
            level_(1),
            experience_(0),
            required_experience_(calculate_required_experience(1)),
            base_max_health_(100),
            max_health_(100),
            current_health_(100),
            base_defense_(10),
            base_damage_(10),
            base_attributes_(100, 10),
            active_weapon_(nullptr),
            active_armor_(nullptr) {
        }

        Alive(const std::string& name, int32_t level,
            int32_t base_health, int32_t base_defense, int32_t base_damage,
            const Attributes& base_attrs) noexcept
            : name_(name),
            level_(level),
            experience_(0),
            required_experience_(calculate_required_experience(level)),
            base_max_health_(base_health),
            max_health_(base_health),
            current_health_(base_health),
            base_defense_(base_defense),
            base_damage_(base_damage),
            base_attributes_(base_attrs),
            active_weapon_(nullptr),
            active_armor_(nullptr) {
        }


        // Rule of five
        Alive(const Alive&) = default;
        Alive(Alive&&) noexcept = default;
        Alive& operator=(const Alive&) = default;
        Alive& operator=(Alive&&) noexcept = default;
        virtual ~Alive() = default;


        // Basic getters
        const std::string& name() const noexcept { return name_; }
        int32_t level() const noexcept { return level_; }
        int64_t experience() const noexcept { return experience_; }
        int64_t required_experience() const noexcept { return required_experience_; }
        int32_t base_max_health() const noexcept { return base_max_health_; }
        int32_t max_health() const noexcept { return max_health_; }
        int32_t current_health() const noexcept { return current_health_; }
        int32_t base_defense() const noexcept { return base_defense_; }
        int32_t base_damage() const noexcept { return base_damage_; }
        const Attributes& base_attributes() const noexcept { return base_attributes_; }
        std::shared_ptr<Weapon> active_weapon() const noexcept { return active_weapon_; }
        std::shared_ptr<Armor> active_armor() const noexcept { return active_armor_; }


        // Basic setters
        void name(const std::string& value) noexcept { name_ = value; }

        ::dungeons::Result<void> level(int32_t value) noexcept {
            if (value < 1) {
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED,
                    "Level must be at least 1");
            }
            level_ = value;
            required_experience_ = calculate_required_experience(level_);
            return ::dungeons::Ok();
        }

        ::dungeons::Result<void> experience(int64_t value) noexcept {
            if (value < 0) {
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED,
                    "Experience cannot be negative");
            }
            experience_ = value;
            return ::dungeons::Ok();
        }

        ::dungeons::Result<void> current_health(int32_t value) noexcept {
            if (value < 0) {
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED,
                    "Health cannot be negative");
            }
            current_health_ = value;
            return ::dungeons::Ok();
        }


        void base_max_health(int32_t value) noexcept { base_max_health_ = value; }
        void base_defense(int32_t value) noexcept { base_defense_ = value; }
        void base_damage(int32_t value) noexcept { base_damage_ = value; }
        void base_attributes(const Attributes& value) noexcept { base_attributes_ = value; }
        void active_weapon(std::shared_ptr<Weapon> value) noexcept { active_weapon_ = value; }
        void active_armor(std::shared_ptr<Armor> value) noexcept { active_armor_ = value; }


        // Calculate current attributes (base + equipment)
        Attributes calculate_current_attributes() const noexcept {
            Attributes result = base_attributes_;
            if (active_armor_) {
                result += active_armor_->attributes();
            }
            if (active_weapon_) {
                result += active_weapon_->attributes();
            }
            return result;
        }

        // Calculate attack (base_damage + weapon + strength)
        int32_t calculate_attack() const noexcept {
            int32_t attack = base_damage_;
            if (active_weapon_) {
                attack += active_weapon_->base_attack();
            }
            auto attrs = calculate_current_attributes();
            attack += attrs.strength;
            return attack;
        }

        // Calculate defense (base_defense + armor + health * 0.1)
        int32_t calculate_defense() const noexcept {
            int32_t defense = base_defense_;
            if (active_armor_) {
                defense += active_armor_->defense();
            }
            auto attrs = calculate_current_attributes();
            defense += static_cast<int32_t>(attrs.health * 0.1f);
            return defense;
        }

        // Calculate max health (base + health_attribute * 10)
        int32_t calculate_max_health() const noexcept {
            auto attrs = calculate_current_attributes();
            return base_max_health_ + attrs.health * 10;
        }

        // Update max health based on current attributes
        void update_max_health() noexcept {
            max_health_ = calculate_max_health();
            if (current_health_ > max_health_) {
                current_health_ = max_health_;
            }
        }

        // Calculate power score (attack * 2 + defense * 4 + health)
        int64_t calculate_power_score() const noexcept {
            int32_t attack = calculate_attack();
            int32_t defense = calculate_defense();
            int32_t health = calculate_max_health();
            return static_cast<int64_t>(attack) * 2 +
                static_cast<int64_t>(defense) * 4 +
                static_cast<int64_t>(health);
        }


        // Check if dead
        bool is_dead() const noexcept {
            return current_health_ <= 0;
        }

        // Heal
        ::dungeons::Result<void> heal(int32_t amount) noexcept {
            if (amount < 0)
                return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT, "Heal amount cannot be negative");
            current_health_ = std::min(current_health_ + amount, max_health_);
            return ::dungeons::Ok();
        }

        // Take damage
        ::dungeons::Result<void> take_damage(int32_t amount) noexcept {
            if (amount < 0)
                return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT, "Damage amount cannot be negative");
            current_health_ = (current_health_ > amount) ? (current_health_ - amount) : 0;
            return ::dungeons::Ok();
        }

        // Heal fully
        void heal_fully() noexcept {
            current_health_ = max_health_;
        }


        // Add experience
        ::dungeons::Result<void> add_experience(int64_t amount) noexcept {
            if (amount < 0) {
                return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT,
                    "Experience amount cannot be negative");
            }

            experience_ += amount;
            return ::dungeons::Ok();
        }


        // Check if can level up
        bool can_level_up() const noexcept {
            return experience_ >= required_experience_;
        }


        // Level up (returns attribute points gained)
        ::dungeons::Result<int32_t> level_up(int32_t attribute_points_per_level = 5) noexcept {
            if (!can_level_up())
                return ::dungeons::Err<int32_t>(::dungeons::ErrorCode::VALIDATION_FAILED, "Not enough experience to level up");
            level_++;
            required_experience_ = calculate_required_experience(level_);
            return ::dungeons::Ok(attribute_points_per_level);
        }


        // Валидация
        ::dungeons::Result<void> validate() const noexcept {
            if (level_ < 1)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Level must be at least 1");
            if (experience_ < 0)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Experience cannot be negative");
            if (current_health_ < 0)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Current health cannot be negative");
            if (max_health_ < 0)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Max health cannot be negative");
            if (current_health_ > max_health_)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Current health exceeds maximum");
            return ::dungeons::Ok();
        }
    };


} // namespace dungeons::backend