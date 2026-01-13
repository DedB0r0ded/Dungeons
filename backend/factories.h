// factories.h
#pragma once

#include "backend_base.h"
#include "ItemMeta.h"
#include "items.h"
#include "Inventory.h"
#include "Enemy.h"
#include "Location.h"
#include "Room.h"

#include <vector>
#include <memory>


namespace dungeons::backend {


    class Player;


    class WeaponFactory {
        std::vector<std::weak_ptr<WeaponMeta>> meta_pool_;

        static constexpr float LOWER_MULTIPLIER = 0.8f;
        static constexpr float UPPER_MULTIPLIER = 1.4f;

    public:
        // Конструкторы
        WeaponFactory() noexcept : meta_pool_() {}

        explicit WeaponFactory(const std::vector<std::weak_ptr<WeaponMeta>>& pool) noexcept
            : meta_pool_(pool) {
        }

        // Правило пяти
        WeaponFactory(const WeaponFactory&) = default;
        WeaponFactory(WeaponFactory&&) noexcept = default;
        WeaponFactory& operator=(const WeaponFactory&) = default;
        WeaponFactory& operator=(WeaponFactory&&) noexcept = default;
        ~WeaponFactory() = default;

        // Геттеры/Сеттеры
        const std::vector<std::weak_ptr<WeaponMeta>>& meta_pool() const noexcept { return meta_pool_; }
        void meta_pool(const std::vector<std::weak_ptr<WeaponMeta>>& pool) noexcept { meta_pool_ = pool; }

        // Создать случайное оружие
        ::dungeons::Result<std::shared_ptr<Weapon>> create_random() noexcept {
            if (meta_pool_.empty())
                return ::dungeons::Err<std::shared_ptr<Weapon>>(
                    ::dungeons::ErrorCode::VALIDATION_FAILED, "Пулл метаданных оружия пуст");
            auto meta_weak = ::dungeons::Random::choice(meta_pool_);
            auto meta = meta_weak.lock();
            if (!meta)
                return ::dungeons::Err<std::shared_ptr<Weapon>>(
                    ::dungeons::ErrorCode::INVALID_ARGUMENT, "Метаданные оружия более не валидны");
            float multiplier = ::dungeons::Random::next_float(LOWER_MULTIPLIER, UPPER_MULTIPLIER);
            int32_t attack = static_cast<int32_t>(meta->attack_basis() * multiplier);
            Attributes attrs = meta->generation_basis() * multiplier;
            int32_t durability = ::dungeons::Random::next_int<int32_t>(50, 150);
            Money value(::dungeons::Random::next_long_long(100, 1000));
            auto weapon = std::make_shared<Weapon>(value, attrs, durability, true, meta_weak, attack);
            return ::dungeons::Ok(weapon);
        }

        // Создать оружие для игрока (масштабируется по уровню)
        ::dungeons::Result<std::shared_ptr<Weapon>> create_for_player(const Player& player) noexcept;
    };

    class ArmorFactory {
        std::vector<std::weak_ptr<ArmorMeta>> meta_pool_;

        static constexpr float LOWER_MULTIPLIER = 0.8f;
        static constexpr float UPPER_MULTIPLIER = 1.4f;

    public:
        // Конструкторы
        ArmorFactory() noexcept : meta_pool_() {}

        explicit ArmorFactory(const std::vector<std::weak_ptr<ArmorMeta>>& pool) noexcept
            : meta_pool_(pool) {
        }

        // Правило пяти
        ArmorFactory(const ArmorFactory&) = default;
        ArmorFactory(ArmorFactory&&) noexcept = default;
        ArmorFactory& operator=(const ArmorFactory&) = default;
        ArmorFactory& operator=(ArmorFactory&&) noexcept = default;
        ~ArmorFactory() = default;

        // Геттеры/Сеттеры
        const std::vector<std::weak_ptr<ArmorMeta>>& meta_pool() const noexcept { return meta_pool_; }
        void meta_pool(const std::vector<std::weak_ptr<ArmorMeta>>& pool) noexcept { meta_pool_ = pool; }

        // Создать случайную броню
        ::dungeons::Result<std::shared_ptr<Armor>> create_random() noexcept {
            if (meta_pool_.empty())
                return ::dungeons::Err<std::shared_ptr<Armor>>(
                    ::dungeons::ErrorCode::VALIDATION_FAILED, "Пулл метаданных брони пуст");
            auto meta_weak = ::dungeons::Random::choice(meta_pool_);
            auto meta = meta_weak.lock();
            if (!meta)
                return ::dungeons::Err<std::shared_ptr<Armor>>(
                    ::dungeons::ErrorCode::INVALID_ARGUMENT, "Метаданные брони более не валидны");
            float multiplier = ::dungeons::Random::next_float(LOWER_MULTIPLIER, UPPER_MULTIPLIER);
            int32_t defense = static_cast<int32_t>(meta->defense_basis() * multiplier);
            Attributes attrs = meta->generation_basis() * multiplier;
            int32_t durability = ::dungeons::Random::next_int<int32_t>(50, 150);
            Money value(::dungeons::Random::next_long_long(100, 1000));
            auto armor = std::make_shared<Armor>(value, attrs, durability, true, meta_weak, defense);
            return ::dungeons::Ok(armor);
        }

        // Создать броню для игрока (масштабируется по уровню)
        ::dungeons::Result<std::shared_ptr<Armor>> create_for_player(const Player& player) noexcept;
    };

    class InventoryFactory {
        std::shared_ptr<WeaponFactory> weapon_factory_;
        std::shared_ptr<ArmorFactory> armor_factory_;

        static constexpr int32_t TREASURE_MIN_ITEMS = 3;
        static constexpr int32_t TREASURE_MAX_ITEMS = 5;
        static constexpr int64_t TREASURE_MONEY_BASIS = 100;
        static constexpr int64_t ENEMY_MONEY_BASIS = 20;

    public:
        // Конструкторы
        InventoryFactory() noexcept
            : weapon_factory_(nullptr), armor_factory_(nullptr) {
        }

        InventoryFactory(std::shared_ptr<WeaponFactory> weapon_factory,
            std::shared_ptr<ArmorFactory> armor_factory) noexcept
            : weapon_factory_(weapon_factory), armor_factory_(armor_factory) {
        }

        // Правило пяти
        InventoryFactory(const InventoryFactory&) = default;
        InventoryFactory(InventoryFactory&&) noexcept = default;
        InventoryFactory& operator=(const InventoryFactory&) = default;
        InventoryFactory& operator=(InventoryFactory&&) noexcept = default;
        ~InventoryFactory() = default;

        // Геттеры/Сеттеры
        std::shared_ptr<WeaponFactory> weapon_factory() const noexcept { return weapon_factory_; }
        std::shared_ptr<ArmorFactory> armor_factory() const noexcept { return armor_factory_; }
        void weapon_factory(std::shared_ptr<WeaponFactory> factory) noexcept { weapon_factory_ = factory; }
        void armor_factory(std::shared_ptr<ArmorFactory> factory) noexcept { armor_factory_ = factory; }

        // Создать инвентарь сокровищ
        Inventory create_treasure(size_t room_id) noexcept {
            int32_t item_count = ::dungeons::Random::next_int<int32_t>(TREASURE_MIN_ITEMS, TREASURE_MAX_ITEMS);
            int64_t money_amount = TREASURE_MONEY_BASIS * static_cast<int64_t>(room_id);
            Money money(money_amount);
            Inventory inventory(20, money);
            // Добавить случайные предметы
            for (int32_t i = 0; i < item_count; ++i) {
                bool is_weapon = ::dungeons::Random::next_bool(0.5);
                if (is_weapon && weapon_factory_) {
                    auto weapon_result = weapon_factory_->create_random();
                    if (weapon_result)
                        inventory.add_item(weapon_result.value());
                }
                else if (!is_weapon && armor_factory_) {
                    auto armor_result = armor_factory_->create_random();
                    if (armor_result)
                        inventory.add_item(armor_result.value());
                }
            }
            return inventory;
        }

        // Создать инвентарь добычи противника
        Inventory create_enemy_loot(size_t room_id) noexcept {
            int64_t money_amount = ENEMY_MONEY_BASIS * static_cast<int64_t>(room_id);
            Money money(money_amount);
            Inventory inventory(10, money);
            // Добавить один случайный предмет
            bool is_weapon = ::dungeons::Random::next_bool(0.5);
            if (is_weapon && weapon_factory_) {
                auto weapon_result = weapon_factory_->create_random();
                if (weapon_result)
                    inventory.add_item(weapon_result.value());
            }
            else if (!is_weapon && armor_factory_) {
                auto armor_result = armor_factory_->create_random();
                if (armor_result)
                    inventory.add_item(armor_result.value());
            }
            return inventory;
        }
    };

    class EnemyFactory {
        std::vector<std::weak_ptr<EnemyMeta>> meta_pool_;
        std::shared_ptr<WeaponFactory> weapon_factory_;
        std::shared_ptr<ArmorFactory> armor_factory_;
        std::shared_ptr<InventoryFactory> inventory_factory_;

        static constexpr float K_MIN = 0.8f;
        static constexpr float K_MAX = 1.2f;

    public:
        // Конструкторы
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

        // Правило пяти
        EnemyFactory(const EnemyFactory&) = default;
        EnemyFactory(EnemyFactory&&) noexcept = default;
        EnemyFactory& operator=(const EnemyFactory&) = default;
        EnemyFactory& operator=(EnemyFactory&&) noexcept = default;
        ~EnemyFactory() = default;

        // Геттеры/Сеттеры
        const std::vector<std::weak_ptr<EnemyMeta>>& meta_pool() const noexcept { return meta_pool_; }
        void meta_pool(const std::vector<std::weak_ptr<EnemyMeta>>& pool) noexcept { meta_pool_ = pool; }

        // Создать врага для игрока
        ::dungeons::Result<Enemy> create_for_player(const Player& player) noexcept;
    };

} // namespace dungeons::backend