// factories.cpp
#include "factories.h"


namespace dungeons::backend {
    
dungeons::Result<std::shared_ptr<Weapon>> WeaponFactory::create_for_player(const PlayerAliveComponent& player) const noexcept {
    if (meta_pool_.empty())
        return ::dungeons::Err<std::shared_ptr<Weapon>>(::dungeons::ErrorCode::VALIDATION_FAILED, "Пулл метаданных оружия пуст");
    auto meta_weak = ::dungeons::Random::choice(meta_pool_);
    auto meta = meta_weak.lock();
    if (!meta)
        return ::dungeons::Err<std::shared_ptr<Weapon>>(::dungeons::ErrorCode::INVALID_ARGUMENT, "Выбранный экземпляр метаданных оружия пуст или не существует");
    float multiplier = ::dungeons::Random::next_float(LOWER_MULTIPLIER, UPPER_MULTIPLIER);
    int32_t attack = static_cast<int32_t>(meta->attack_basis() * player.level() * multiplier);
    Attributes attrs = meta->generation_basis() * (player.level() * multiplier);
    int32_t durability = ::dungeons::Random::next_int<int32_t>(50, 150);
    Money value(::dungeons::Random::next_long_long(100 * player.level(), 1000 * player.level()));
    auto weapon = std::make_shared<Weapon>(value, attrs, durability, true, meta_weak, attack);
    return ::dungeons::Ok(weapon);
}

dungeons::Result<std::shared_ptr<Armor>> ArmorFactory::create_for_player(const PlayerAliveComponent& player) const noexcept {
    if (meta_pool_.empty())
        return ::dungeons::Err<std::shared_ptr<Armor>>(::dungeons::ErrorCode::VALIDATION_FAILED, "Пулл метаданных брони пуст");
    auto meta_weak = ::dungeons::Random::choice(meta_pool_);
    auto meta = meta_weak.lock();
    if (!meta)
        return ::dungeons::Err<std::shared_ptr<Armor>>(::dungeons::ErrorCode::INVALID_ARGUMENT, "Выбранный экземпляр метаданных брони пуст или не существует");
    float multiplier = ::dungeons::Random::next_float(LOWER_MULTIPLIER, UPPER_MULTIPLIER);
    int32_t defense = static_cast<int32_t>(meta->defense_basis() * player.level() * multiplier);
    Attributes attrs = meta->generation_basis() * (player.level() * multiplier);
    int32_t durability = ::dungeons::Random::next_int<int32_t>(50, 150);
    Money value(::dungeons::Random::next_long_long(100 * player.level(), 1000 * player.level()));
    auto armor = std::make_shared<Armor>(value, attrs, durability, true, meta_weak, defense);
    return ::dungeons::Ok(armor);
}

dungeons::Result<Enemy> EnemyFactory::create_for_player(const PlayerAliveComponent& player) const noexcept {
    if (meta_pool_.empty())
        return ::dungeons::Err<Enemy>(::dungeons::ErrorCode::VALIDATION_FAILED, "Пулл метаданных врагов пуст");
    auto meta_weak = ::dungeons::Random::choice(meta_pool_);
    auto meta = meta_weak.lock();
    if (!meta)
        return ::dungeons::Err<Enemy>(::dungeons::ErrorCode::INVALID_ARGUMENT, "Выбранный экземпляр метаданных пуст или не существует");
    float k = ::dungeons::Random::next_float(K_MIN, K_MAX);
    float generation_basis = meta->generation_basis();
    int32_t enemy_health = static_cast<int32_t>(player.base_max_health() * k * generation_basis);
    int32_t enemy_defense = static_cast<int32_t>(player.base_defense() * k * generation_basis);
    int32_t enemy_damage = static_cast<int32_t>(player.base_damage() * k * generation_basis);
    Attributes enemy_attrs = player.base_attributes() * (k * generation_basis);
    Enemy enemy(meta->name(), player.level(), enemy_health, enemy_defense,
        enemy_damage, enemy_attrs, 10, meta_weak);
    if (weapon_factory_) {
        auto weapon_result = weapon_factory_->create_for_player(player);
        if (weapon_result)
            enemy.active_weapon(weapon_result.value());
    }
    if (armor_factory_) {
        auto armor_result = armor_factory_->create_for_player(player);
        if (armor_result)
            enemy.active_armor(armor_result.value());
    }
    if (inventory_factory_) {
        auto loot = inventory_factory_->create_enemy_loot(player.level());
        enemy.inventory().transfer_all(loot);
    }
    enemy.heal_fully();
    enemy.update_max_health();
    return ::dungeons::Ok(enemy);
}


dungeons::Result<std::shared_ptr<Weapon>> WeaponFactory::create_random() const noexcept {
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


dungeons::Result<std::shared_ptr<Armor>> ArmorFactory::create_random() const noexcept {
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


dungeons::backend::Inventory InventoryFactory::create_treasure(size_t room_id) const noexcept {
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

dungeons::backend::Inventory InventoryFactory::create_enemy_loot(size_t room_id) const noexcept {
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

} // namespace dungeons::backend
