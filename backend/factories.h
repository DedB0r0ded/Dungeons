// Factories.h
// TODO: fix newlines
// TODO: translate to russian (comments and error messages)
// TODO: remove redundant private modifiers
#pragma once


#include "./ItemMeta.h"
#include "./Item.h"
#include "./Inventory.h"
#include "./Enemy.h"
#include "./Player.h"
#include "./backend_base.h"

#include <vector>
#include <memory>


namespace dungeons::backend {


    // Forward declaration
    class Player;

    class WeaponFactory {
    private:
        std::vector<std::weak_ptr<WeaponMeta>> meta_pool_;

        static constexpr float LOWER_MULTIPLIER = 0.8f;
        static constexpr float UPPER_MULTIPLIER = 1.4f;

    public:
        // Constructors
        WeaponFactory() noexcept : meta_pool_() {}

        explicit WeaponFactory(const std::vector<std::weak_ptr<WeaponMeta>>& pool) noexcept
            : meta_pool_(pool) {
        }

        // Rule of five
        WeaponFactory(const WeaponFactory&) = default;
        WeaponFactory(WeaponFactory&&) noexcept = default;
        WeaponFactory& operator=(const WeaponFactory&) = default;
        WeaponFactory& operator=(WeaponFactory&&) noexcept = default;
        ~WeaponFactory() = default;

        // Getters/Setters
        const std::vector<std::weak_ptr<WeaponMeta>>& meta_pool() const noexcept { return meta_pool_; }
        void meta_pool(const std::vector<std::weak_ptr<WeaponMeta>>& pool) noexcept { meta_pool_ = pool; }

        // Create random weapon
        ::dungeons::Result<std::shared_ptr<Weapon>> create_random() noexcept {
            if (meta_pool_.empty()) {
                return ::dungeons::Err<std::shared_ptr<Weapon>>(
                    ::dungeons::ErrorCode::VALIDATION_FAILED,
                    "Weapon meta pool is empty");
            }

            auto meta_weak = ::dungeons::Random::choice(meta_pool_);
            auto meta = meta_weak.lock();
            if (!meta) {
                return ::dungeons::Err<std::shared_ptr<Weapon>>(
                    ::dungeons::ErrorCode::INVALID_ARGUMENT,
                    "Weapon meta is no longer valid");
            }

            float multiplier = ::dungeons::Random::next_float(LOWER_MULTIPLIER, UPPER_MULTIPLIER);

            int32_t attack = static_cast<int32_t>(meta->attack_basis() * multiplier);
            Attributes attrs = meta->generation_basis() * multiplier;
            int32_t durability = ::dungeons::Random::next_int<int32_t>(50, 150);
            Money value(::dungeons::Random::next_long_long(100, 1000));

            auto weapon = std::make_shared<Weapon>(
                value, attrs, durability, true, meta_weak, attack);

            return ::dungeons::Ok(weapon);
        }

        // Create weapon for player (scaled to player level)
        ::dungeons::Result<std::shared_ptr<Weapon>> create_for_player(const Player& player) noexcept {
            if (meta_pool_.empty()) {
                return ::dungeons::Err<std::shared_ptr<Weapon>>(
                    ::dungeons::ErrorCode::VALIDATION_FAILED,
                    "Weapon meta pool is empty");
            }

            auto meta_weak = ::dungeons::Random::choice(meta_pool_);
            auto meta = meta_weak.lock();
            if (!meta) {
                return ::dungeons::Err<std::shared_ptr<Weapon>>(
                    ::dungeons::ErrorCode::INVALID_ARGUMENT,
                    "Weapon meta is no longer valid");
            }

            float multiplier = ::dungeons::Random::next_float(LOWER_MULTIPLIER, UPPER_MULTIPLIER);

            int32_t attack = static_cast<int32_t>(
                meta->attack_basis() * player.level() * multiplier);
            Attributes attrs = meta->generation_basis() * (player.level() * multiplier);
            int32_t durability = ::dungeons::Random::next_int<int32_t>(50, 150);
            Money value(::dungeons::Random::next_long_long(100 * player.level(),
                1000 * player.level()));

            auto weapon = std::make_shared<Weapon>(
                value, attrs, durability, true, meta_weak, attack);

            return ::dungeons::Ok(weapon);
        }
    };

    class ArmorFactory {
    private:
        std::vector<std::weak_ptr<ArmorMeta>> meta_pool_;

        static constexpr float LOWER_MULTIPLIER = 0.8f;
        static constexpr float UPPER_MULTIPLIER = 1.4f;

    public:
        // Constructors
        ArmorFactory() noexcept : meta_pool_() {}

        explicit ArmorFactory(const std::vector<std::weak_ptr<ArmorMeta>>& pool) noexcept
            : meta_pool_(pool) {
        }

        // Rule of five
        ArmorFactory(const ArmorFactory&) = default;
        ArmorFactory(ArmorFactory&&) noexcept = default;
        ArmorFactory& operator=(const ArmorFactory&) = default;
        ArmorFactory& operator=(ArmorFactory&&) noexcept = default;
        ~ArmorFactory() = default;

        // Getters/Setters
        const std::vector<std::weak_ptr<ArmorMeta>>& meta_pool() const noexcept { return meta_pool_; }
        void meta_pool(const std::vector<std::weak_ptr<ArmorMeta>>& pool) noexcept { meta_pool_ = pool; }

        // Create random armor
        ::dungeons::Result<std::shared_ptr<Armor>> create_random() noexcept {
            if (meta_pool_.empty()) {
                return ::dungeons::Err<std::shared_ptr<Armor>>(
                    ::dungeons::ErrorCode::VALIDATION_FAILED,
                    "Armor meta pool is empty");
            }

            auto meta_weak = ::dungeons::Random::choice(meta_pool_);
            auto meta = meta_weak.lock();
            if (!meta) {
                return ::dungeons::Err<std::shared_ptr<Armor>>(
                    ::dungeons::ErrorCode::INVALID_ARGUMENT,
                    "Armor meta is no longer valid");
            }

            float multiplier = ::dungeons::Random::next_float(LOWER_MULTIPLIER, UPPER_MULTIPLIER);

            int32_t defense = static_cast<int32_t>(meta->defense_basis() * multiplier);
            Attributes attrs = meta->generation_basis() * multiplier;
            int32_t durability = ::dungeons::Random::next_int<int32_t>(50, 150);
            Money value(::dungeons::Random::next_long_long(100, 1000));

            auto armor = std::make_shared<Armor>(
                value, attrs, durability, true, meta_weak, defense);

            return ::dungeons::Ok(armor);
        }

        // Create armor for player (scaled to player level)
        ::dungeons::Result<std::shared_ptr<Armor>> create_for_player(const Player& player) noexcept {
            if (meta_pool_.empty()) {
                return ::dungeons::Err<std::shared_ptr<Armor>>(
                    ::dungeons::ErrorCode::VALIDATION_FAILED,
                    "Armor meta pool is empty");
            }

            auto meta_weak = ::dungeons::Random::choice(meta_pool_);
            auto meta = meta_weak.lock();
            if (!meta) {
                return ::dungeons::Err<std::shared_ptr<Armor>>(
                    ::dungeons::ErrorCode::INVALID_ARGUMENT,
                    "Armor meta is no longer valid");
            }

            float multiplier = ::dungeons::Random::next_float(LOWER_MULTIPLIER, UPPER_MULTIPLIER);

            int32_t defense = static_cast<int32_t>(
                meta->defense_basis() * player.level() * multiplier);
            Attributes attrs = meta->generation_basis() * (player.level() * multiplier);
            int32_t durability = ::dungeons::Random::next_int<int32_t>(50, 150);
            Money value(::dungeons::Random::next_long_long(100 * player.level(),
                1000 * player.level()));

            auto armor = std::make_shared<Armor>(
                value, attrs, durability, true, meta_weak, defense);

            return ::dungeons::Ok(armor);
        }
    };

    class InventoryFactory {
    private:
        std::shared_ptr<WeaponFactory> weapon_factory_;
        std::shared_ptr<ArmorFactory> armor_factory_;

        static constexpr int32_t TREASURE_MIN_ITEMS = 3;
        static constexpr int32_t TREASURE_MAX_ITEMS = 5;
        static constexpr int64_t TREASURE_MONEY_BASIS = 100;
        static constexpr int64_t ENEMY_MONEY_BASIS = 20;

    public:
        // Constructors
        InventoryFactory() noexcept
            : weapon_factory_(nullptr), armor_factory_(nullptr) {
        }

        InventoryFactory(std::shared_ptr<WeaponFactory> weapon_factory,
            std::shared_ptr<ArmorFactory> armor_factory) noexcept
            : weapon_factory_(weapon_factory), armor_factory_(armor_factory) {
        }

        // Rule of five
        InventoryFactory(const InventoryFactory&) = default;
        InventoryFactory(InventoryFactory&&) noexcept = default;
        InventoryFactory& operator=(const InventoryFactory&) = default;
        InventoryFactory& operator=(InventoryFactory&&) noexcept = default;
        ~InventoryFactory() = default;

        // Getters/Setters
        std::shared_ptr<WeaponFactory> weapon_factory() const noexcept { return weapon_factory_; }
        std::shared_ptr<ArmorFactory> armor_factory() const noexcept { return armor_factory_; }
        void weapon_factory(std::shared_ptr<WeaponFactory> factory) noexcept { weapon_factory_ = factory; }
        void armor_factory(std::shared_ptr<ArmorFactory> factory) noexcept { armor_factory_ = factory; }

        // Create treasure inventory
        Inventory create_treasure(size_t room_id) noexcept {
            int32_t item_count = ::dungeons::Random::next_int<int32_t>(
                TREASURE_MIN_ITEMS, TREASURE_MAX_ITEMS);

            int64_t money_amount = TREASURE_MONEY_BASIS * static_cast<int64_t>(room_id);
            Money money(money_amount);

            Inventory inventory(20, money);

            // Add random items
            for (int32_t i = 0; i < item_count; ++i) {
                bool is_weapon = ::dungeons::Random::next_bool(0.5);

                if (is_weapon && weapon_factory_) {
                    auto weapon_result = weapon_factory_->create_random();
                    if (weapon_result) {
                        inventory.add_item(weapon_result.value());
                    }
                }
                else if (!is_weapon && armor_factory_) {
                    auto armor_result = armor_factory_->create_random();
                    if (armor_result) {
                        inventory.add_item(armor_result.value());
                    }
                }
            }

            return inventory;
        }

        // Create enemy loot inventory
        Inventory create_enemy_loot(size_t room_id) noexcept {
            int64_t money_amount = ENEMY_MONEY_BASIS * static_cast<int64_t>(room_id);
            Money money(money_amount);

            Inventory inventory(10, money);

            // Add one random item
            bool is_weapon = ::dungeons::Random::next_bool(0.5);

            if (is_weapon && weapon_factory_) {
                auto weapon_result = weapon_factory_->create_random();
                if (weapon_result) {
                    inventory.add_item(weapon_result.value());
                }
            }
            else if (!is_weapon && armor_factory_) {
                auto armor_result = armor_factory_->create_random();
                if (armor_result) {
                    inventory.add_item(armor_result.value());
                }
            }

            return inventory;
        }
    };

    class EnemyFactory {
    private:
        std::vector<std::weak_ptr<EnemyMeta>> meta_pool_;
        std::shared_ptr<WeaponFactory> weapon_factory_;
        std::shared_ptr<ArmorFactory> armor_factory_;
        std::shared_ptr<InventoryFactory> inventory_factory_;

        static constexpr float K_MIN = 0.8f;
        static constexpr float K_MAX = 1.2f;

    public:
        // Constructors
        EnemyFactory() noexcept
            : meta_pool_(),
            weapon_factory_(nullptr),
            armor_factory_(nullptr),
            inventory_factory_(nullptr) {
        }

        EnemyFactory(const std::vector<std::weak_ptr<EnemyMeta>>& pool,
            std::shared_ptr<WeaponFactory> weapon_factory,
            std::shared_ptr<ArmorFactory> armor_factory,
            std::shared_ptr<InventoryFactory> inventory_factory) noexcept
            : meta_pool_(pool),
            weapon_factory_(weapon_factory),
            armor_factory_(armor_factory),
            inventory_factory_(inventory_factory) {
        }

        // Rule of five
        EnemyFactory(const EnemyFactory&) = default;
        EnemyFactory(EnemyFactory&&) noexcept = default;
        EnemyFactory& operator=(const EnemyFactory&) = default;
        EnemyFactory& operator=(EnemyFactory&&) noexcept = default;
        ~EnemyFactory() = default;

        // Getters/Setters
        const std::vector<std::weak_ptr<EnemyMeta>>& meta_pool() const noexcept { return meta_pool_; }
        void meta_pool(const std::vector<std::weak_ptr<EnemyMeta>>& pool) noexcept { meta_pool_ = pool; }

        // Create enemy for player
        ::dungeons::Result<Enemy> create_for_player(const Player& player) noexcept {
            if (meta_pool_.empty()) {
                return ::dungeons::Err<Enemy>(
                    ::dungeons::ErrorCode::VALIDATION_FAILED,
                    "Enemy meta pool is empty");
            }

            auto meta_weak = ::dungeons::Random::choice(meta_pool_);
            auto meta = meta_weak.lock();
            if (!meta) {
                return ::dungeons::Err<Enemy>(
                    ::dungeons::ErrorCode::INVALID_ARGUMENT,
                    "Enemy meta is no longer valid");
            }

            // Generate k coefficient
            float k = ::dungeons::Random::next_float(K_MIN, K_MAX);
            float generation_basis = meta->generation_basis();

            // Calculate enemy stats based on player
            int32_t enemy_health = static_cast<int32_t>(
                player.base_max_health() * k * generation_basis);
            int32_t enemy_defense = static_cast<int32_t>(
                player.base_defense() * k * generation_basis);
            int32_t enemy_damage = static_cast<int32_t>(
                player.base_damage() * k * generation_basis);

            Attributes enemy_attrs = player.base_attributes() * (k * generation_basis);

            // Create enemy
            Enemy enemy(meta->name(), player.level(), enemy_health, enemy_defense,
                enemy_damage, enemy_attrs, 10, meta_weak);

            // Equip weapon
            if (weapon_factory_) {
                auto weapon_result = weapon_factory_->create_for_player(player);
                if (weapon_result) {
                    enemy.active_weapon(weapon_result.value());
                }
            }

            // Equip armor
            if (armor_factory_) {
                auto armor_result = armor_factory_->create_for_player(player);
                if (armor_result) {
                    enemy.active_armor(armor_result.value());
                }
            }

            // Fill inventory
            if (inventory_factory_) {
                auto loot = inventory_factory_->create_enemy_loot(player.level());
                enemy.inventory().transfer_all(loot);
            }

            // Set to full health
            enemy.heal_fully();
            enemy.update_max_health();

            return ::dungeons::Ok(enemy);
        }
    };

} // namespace dungeons::backend