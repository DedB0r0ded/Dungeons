// uid_test.cpp
#include <gtest/gtest.h>
#include "../backend/uid.h"
#include <thread>
#include <chrono>
#include <set>

using namespace dungeons::backend;

// ============================================================================
// UidFlags Tests
// ============================================================================

TEST(UidFlagsTest, BitwiseOR) {
    UidFlags flags = UidFlags::IS_PLAYER | UidFlags::IS_WEAPON;
    EXPECT_TRUE(has_flag(flags, UidFlags::IS_PLAYER));
    EXPECT_TRUE(has_flag(flags, UidFlags::IS_WEAPON));
    EXPECT_FALSE(has_flag(flags, UidFlags::IS_ENEMY));
}

TEST(UidFlagsTest, BitwiseAND) {
    UidFlags flags = UidFlags::IS_PLAYER | UidFlags::IS_WEAPON;
    UidFlags result = flags & UidFlags::IS_PLAYER;
    EXPECT_TRUE(has_flag(result, UidFlags::IS_PLAYER));
}

TEST(UidFlagsTest, BitwiseXOR) {
    UidFlags flags = UidFlags::IS_PLAYER;
    flags ^= UidFlags::IS_PLAYER; // Toggle off
    EXPECT_FALSE(has_flag(flags, UidFlags::IS_PLAYER));
}

TEST(UidFlagsTest, BitwiseNOT) {
    UidFlags flags = UidFlags::IS_PLAYER;
    UidFlags inverted = ~flags;
    EXPECT_FALSE(has_flag(inverted, UidFlags::IS_PLAYER));
}

TEST(UidFlagsTest, CompoundAssignmentOR) {
    UidFlags flags = UidFlags::IS_PLAYER;
    flags |= UidFlags::IS_WEAPON;
    EXPECT_TRUE(has_flag(flags, UidFlags::IS_PLAYER));
    EXPECT_TRUE(has_flag(flags, UidFlags::IS_WEAPON));
}

TEST(UidFlagsTest, CompoundAssignmentAND) {
    UidFlags flags = UidFlags::IS_PLAYER | UidFlags::IS_WEAPON;
    flags &= UidFlags::IS_PLAYER;
    EXPECT_TRUE(has_flag(flags, UidFlags::IS_PLAYER));
    EXPECT_FALSE(has_flag(flags, UidFlags::IS_WEAPON));
}

TEST(UidFlagsTest, CompoundAssignmentXOR) {
    UidFlags flags = UidFlags::IS_PLAYER;
    flags ^= UidFlags::IS_PLAYER; // Toggle
    EXPECT_FALSE(has_flag(flags, UidFlags::IS_PLAYER));
}

TEST(UidFlagsTest, HasFlagNone) {
    UidFlags flags = UidFlags::NONE;
    EXPECT_FALSE(has_flag(flags, UidFlags::IS_PLAYER));
    EXPECT_FALSE(has_flag(flags, UidFlags::IS_ENEMY));
}

TEST(UidFlagsTest, MultipleFlagsCombination) {
    UidFlags flags = UidFlags::IS_ITEM | UidFlags::IS_WEAPON | UidFlags::IS_ARMOR;
    EXPECT_TRUE(has_flag(flags, UidFlags::IS_ITEM));
    EXPECT_TRUE(has_flag(flags, UidFlags::IS_WEAPON));
    EXPECT_TRUE(has_flag(flags, UidFlags::IS_ARMOR));
    EXPECT_FALSE(has_flag(flags, UidFlags::IS_PLAYER));
}

TEST(UidFlagsTest, AllFlagsCombination) {
    UidFlags flags = UidFlags::IS_PLAYER | UidFlags::IS_ENEMY | UidFlags::IS_WEAPON |
        UidFlags::IS_ARMOR | UidFlags::IS_INVENTORY | UidFlags::IS_ITEM |
        UidFlags::IS_LOCATION | UidFlags::IS_ROOM;

    EXPECT_TRUE(has_flag(flags, UidFlags::IS_PLAYER));
    EXPECT_TRUE(has_flag(flags, UidFlags::IS_ENEMY));
    EXPECT_TRUE(has_flag(flags, UidFlags::IS_WEAPON));
    EXPECT_TRUE(has_flag(flags, UidFlags::IS_ARMOR));
    EXPECT_TRUE(has_flag(flags, UidFlags::IS_INVENTORY));
    EXPECT_TRUE(has_flag(flags, UidFlags::IS_ITEM));
    EXPECT_TRUE(has_flag(flags, UidFlags::IS_LOCATION));
    EXPECT_TRUE(has_flag(flags, UidFlags::IS_ROOM));
}

// ============================================================================
// uid_t Construction Tests
// ============================================================================

TEST(UidTest, DefaultConstructor) {
    uid_t uid;
    EXPECT_EQ(uid.seed_id(), 0);
    EXPECT_EQ(uid.flags(), UidFlags::NONE);
    EXPECT_EQ(uid.random_id(), 0);
}

TEST(UidTest, ParameterizedConstructor) {
    auto time_result = dungeons::Time::now();
    ASSERT_TRUE(time_result);

    uid_t uid(42, UidFlags::IS_PLAYER, time_result.value(), 12345);

    EXPECT_EQ(uid.seed_id(), 42);
    EXPECT_TRUE(uid.is_player());
    EXPECT_EQ(uid.random_id(), 12345);
}

TEST(UidTest, GenerateWithSeedAndFlags) {
    auto uid = uid_t::generate(100, UidFlags::IS_ENEMY);

    EXPECT_EQ(uid.seed_id(), 100);
    EXPECT_TRUE(uid.is_enemy());
    EXPECT_NE(uid.random_id(), 0); // Should be random
}

TEST(UidTest, GenerateMultipleUnique) {
    auto uid1 = uid_t::generate(1, UidFlags::IS_PLAYER);
    auto uid2 = uid_t::generate(1, UidFlags::IS_PLAYER);

    // Random IDs should be different
    EXPECT_NE(uid1.random_id(), uid2.random_id());
}

TEST(UidTest, FromRepository) {
    auto time_result = dungeons::Time::now();
    ASSERT_TRUE(time_result);

    auto uid = uid_t::from_repository(999, UidFlags::IS_WEAPON, time_result.value(), 77777);

    EXPECT_EQ(uid.seed_id(), 999);
    EXPECT_TRUE(uid.is_weapon());
    EXPECT_EQ(uid.random_id(), 77777);
}

// ============================================================================
// uid_t Copy and Move Semantics Tests
// ============================================================================

TEST(UidTest, CopyConstructor) {
    auto uid1 = uid_t::generate(50, UidFlags::IS_ARMOR);
    uid_t uid2(uid1);

    EXPECT_EQ(uid1.seed_id(), uid2.seed_id());
    EXPECT_EQ(uid1.flags(), uid2.flags());
    EXPECT_EQ(uid1.random_id(), uid2.random_id());
    EXPECT_EQ(uid1.timestamp(), uid2.timestamp());
}

TEST(UidTest, CopyAssignment) {
    auto uid1 = uid_t::generate(50, UidFlags::IS_ARMOR);
    uid_t uid2;
    uid2 = uid1;

    EXPECT_EQ(uid1.seed_id(), uid2.seed_id());
    EXPECT_EQ(uid1.flags(), uid2.flags());
    EXPECT_EQ(uid1.random_id(), uid2.random_id());
}

TEST(UidTest, MoveConstructor) {
    auto uid1 = uid_t::generate(50, UidFlags::IS_ARMOR);
    auto original_seed = uid1.seed_id();
    auto original_random = uid1.random_id();

    uid_t uid2(std::move(uid1));

    EXPECT_EQ(uid2.seed_id(), original_seed);
    EXPECT_EQ(uid2.random_id(), original_random);
}

TEST(UidTest, MoveAssignment) {
    auto uid1 = uid_t::generate(50, UidFlags::IS_ARMOR);
    auto original_seed = uid1.seed_id();
    auto original_random = uid1.random_id();

    uid_t uid2;
    uid2 = std::move(uid1);

    EXPECT_EQ(uid2.seed_id(), original_seed);
    EXPECT_EQ(uid2.random_id(), original_random);
}

// ============================================================================
// uid_t Getters and Setters Tests
// ============================================================================

TEST(UidTest, SetSeedId) {
    uid_t uid;
    uid.seed_id(123);
    EXPECT_EQ(uid.seed_id(), 123);
}

TEST(UidTest, SetFlags) {
    uid_t uid;
    uid.flags(UidFlags::IS_INVENTORY);
    EXPECT_EQ(uid.flags(), UidFlags::IS_INVENTORY);
}

TEST(UidTest, SetRandomId) {
    uid_t uid;
    uid.random_id(999888777);
    EXPECT_EQ(uid.random_id(), 999888777);
}

TEST(UidTest, SetTimestamp) {
    uid_t uid;
    auto time_result = dungeons::Time::now();
    ASSERT_TRUE(time_result);

    uid.timestamp(time_result.value());
    EXPECT_EQ(uid.timestamp(), time_result.value());
}

// ============================================================================
// uid_t Flag Operations Tests
// ============================================================================

TEST(UidTest, HasFlag) {
    auto uid = uid_t::generate(1, UidFlags::IS_PLAYER);
    EXPECT_TRUE(uid.has_flag(UidFlags::IS_PLAYER));
    EXPECT_FALSE(uid.has_flag(UidFlags::IS_ENEMY));
}

TEST(UidTest, SetFlag) {
    uid_t uid;
    uid.set_flag(UidFlags::IS_WEAPON);
    EXPECT_TRUE(uid.has_flag(UidFlags::IS_WEAPON));
    EXPECT_TRUE(uid.is_weapon());
}

TEST(UidTest, ClearFlag) {
    auto uid = uid_t::generate(1, UidFlags::IS_PLAYER);
    EXPECT_TRUE(uid.is_player());

    uid.clear_flag(UidFlags::IS_PLAYER);
    EXPECT_FALSE(uid.is_player());
}

TEST(UidTest, ToggleFlag) {
    uid_t uid;
    EXPECT_FALSE(uid.is_armor());

    uid.toggle_flag(UidFlags::IS_ARMOR);
    EXPECT_TRUE(uid.is_armor());

    uid.toggle_flag(UidFlags::IS_ARMOR);
    EXPECT_FALSE(uid.is_armor());
}

TEST(UidTest, SetMultipleFlags) {
    uid_t uid;
    uid.set_flag(UidFlags::IS_ITEM);
    uid.set_flag(UidFlags::IS_WEAPON);
    uid.set_flag(UidFlags::IS_ARMOR);

    EXPECT_TRUE(uid.is_item());
    EXPECT_TRUE(uid.is_weapon());
    EXPECT_TRUE(uid.is_armor());
}

TEST(UidTest, ClearSpecificFlagFromMultiple) {
    uid_t uid;
    uid.set_flag(UidFlags::IS_ITEM);
    uid.set_flag(UidFlags::IS_WEAPON);

    uid.clear_flag(UidFlags::IS_WEAPON);

    EXPECT_TRUE(uid.is_item());
    EXPECT_FALSE(uid.is_weapon());
}

// ============================================================================
// uid_t Boolean Getters Tests
// ============================================================================

TEST(UidTest, IsPlayer) {
    auto uid = uid_t::generate(1, UidFlags::IS_PLAYER);
    EXPECT_TRUE(uid.is_player());
    EXPECT_FALSE(uid.is_enemy());
}

TEST(UidTest, IsEnemy) {
    auto uid = uid_t::generate(1, UidFlags::IS_ENEMY);
    EXPECT_TRUE(uid.is_enemy());
    EXPECT_FALSE(uid.is_player());
}

TEST(UidTest, IsWeapon) {
    auto uid = uid_t::generate(1, UidFlags::IS_WEAPON);
    EXPECT_TRUE(uid.is_weapon());
    EXPECT_FALSE(uid.is_armor());
}

TEST(UidTest, IsArmor) {
    auto uid = uid_t::generate(1, UidFlags::IS_ARMOR);
    EXPECT_TRUE(uid.is_armor());
    EXPECT_FALSE(uid.is_weapon());
}

TEST(UidTest, IsInventory) {
    auto uid = uid_t::generate(1, UidFlags::IS_INVENTORY);
    EXPECT_TRUE(uid.is_inventory());
    EXPECT_FALSE(uid.is_item());
}

TEST(UidTest, IsItem) {
    auto uid = uid_t::generate(1, UidFlags::IS_ITEM);
    EXPECT_TRUE(uid.is_item());
    EXPECT_FALSE(uid.is_inventory());
}

TEST(UidTest, IsLocation) {
    auto uid = uid_t::generate(1, UidFlags::IS_LOCATION);
    EXPECT_TRUE(uid.is_location());
    EXPECT_FALSE(uid.is_room());
}

TEST(UidTest, IsRoom) {
    auto uid = uid_t::generate(1, UidFlags::IS_ROOM);
    EXPECT_TRUE(uid.is_room());
    EXPECT_FALSE(uid.is_location());
}

// ============================================================================
// uid_t Boolean Setters Tests
// ============================================================================

TEST(UidTest, SetIsPlayerTrue) {
    uid_t uid;
    uid.is_player(true);
    EXPECT_TRUE(uid.is_player());
}

TEST(UidTest, SetIsPlayerFalse) {
    auto uid = uid_t::generate(1, UidFlags::IS_PLAYER);
    uid.is_player(false);
    EXPECT_FALSE(uid.is_player());
}

TEST(UidTest, SetIsEnemyTrue) {
    uid_t uid;
    uid.is_enemy(true);
    EXPECT_TRUE(uid.is_enemy());
}

TEST(UidTest, SetIsWeaponTrue) {
    uid_t uid;
    uid.is_weapon(true);
    EXPECT_TRUE(uid.is_weapon());
}

TEST(UidTest, SetIsArmorTrue) {
    uid_t uid;
    uid.is_armor(true);
    EXPECT_TRUE(uid.is_armor());
}

TEST(UidTest, SetIsInventoryTrue) {
    uid_t uid;
    uid.is_inventory(true);
    EXPECT_TRUE(uid.is_inventory());
}

TEST(UidTest, SetIsItemTrue) {
    uid_t uid;
    uid.is_item(true);
    EXPECT_TRUE(uid.is_item());
}

TEST(UidTest, SetIsLocationTrue) {
    uid_t uid;
    uid.is_location(true);
    EXPECT_TRUE(uid.is_location());
}

TEST(UidTest, SetIsRoomTrue) {
    uid_t uid;
    uid.is_room(true);
    EXPECT_TRUE(uid.is_room());
}

TEST(UidTest, SetAllBooleanFlags) {
    uid_t uid;
    uid.is_player(true);
    uid.is_enemy(true);
    uid.is_weapon(true);
    uid.is_armor(true);
    uid.is_inventory(true);
    uid.is_item(true);
    uid.is_location(true);
    uid.is_room(true);

    EXPECT_TRUE(uid.is_player());
    EXPECT_TRUE(uid.is_enemy());
    EXPECT_TRUE(uid.is_weapon());
    EXPECT_TRUE(uid.is_armor());
    EXPECT_TRUE(uid.is_inventory());
    EXPECT_TRUE(uid.is_item());
    EXPECT_TRUE(uid.is_location());
    EXPECT_TRUE(uid.is_room());
}

TEST(UidTest, ToggleBooleanFlags) {
    uid_t uid;

    uid.is_player(true);
    EXPECT_TRUE(uid.is_player());

    uid.is_player(false);
    EXPECT_FALSE(uid.is_player());

    uid.is_player(true);
    EXPECT_TRUE(uid.is_player());
}

// ============================================================================
// uid_t Comparison Tests
// ============================================================================

TEST(UidTest, EqualityOperator) {
    auto uid1 = uid_t::generate(100, UidFlags::IS_PLAYER);
    auto uid2 = uid1; // Copy

    EXPECT_TRUE(uid1 == uid2);
}

TEST(UidTest, InequalityOperator) {
    auto uid1 = uid_t::generate(100, UidFlags::IS_PLAYER);
    auto uid2 = uid_t::generate(100, UidFlags::IS_PLAYER);

    EXPECT_TRUE(uid1 != uid2);
}

TEST(UidTest, EqualityDifferentSeedId) {
    auto time_result = dungeons::Time::now();
    ASSERT_TRUE(time_result);

    uid_t uid1(100, UidFlags::IS_PLAYER, time_result.value(), 12345);
    uid_t uid2(200, UidFlags::IS_PLAYER, time_result.value(), 12345);

    EXPECT_FALSE(uid1 == uid2);
    EXPECT_TRUE(uid1 != uid2);
}

TEST(UidTest, EqualityDifferentFlags) {
    auto time_result = dungeons::Time::now();
    ASSERT_TRUE(time_result);

    uid_t uid1(100, UidFlags::IS_PLAYER, time_result.value(), 12345);
    uid_t uid2(100, UidFlags::IS_ENEMY, time_result.value(), 12345);

    EXPECT_FALSE(uid1 == uid2);
}

TEST(UidTest, EqualityDifferentRandomId) {
    auto time_result = dungeons::Time::now();
    ASSERT_TRUE(time_result);

    uid_t uid1(100, UidFlags::IS_PLAYER, time_result.value(), 12345);
    uid_t uid2(100, UidFlags::IS_PLAYER, time_result.value(), 67890);

    EXPECT_FALSE(uid1 == uid2);
}

// ============================================================================
// uid_t Serialization Tests
// ============================================================================

TEST(UidTest, ToBytes) {
    auto uid = uid_t::generate(42, UidFlags::IS_WEAPON);
    uint8_t buffer[sizeof(uid_t)];

    uid.to_bytes(buffer);

    // Should not crash
    SUCCEED();
}

TEST(UidTest, FromBytes) {
    auto uid1 = uid_t::generate(42, UidFlags::IS_WEAPON);
    uint8_t buffer[sizeof(uid_t)];

    uid1.to_bytes(buffer);
    auto result = uid_t::from_bytes(buffer);

    ASSERT_TRUE(result);
    auto uid2 = result.value();

    EXPECT_EQ(uid1, uid2);
}

TEST(UidTest, FromBytesNullBuffer) {
    auto result = uid_t::from_bytes(nullptr);
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error().code(), dungeons::ErrorCode::INVALID_ARGUMENT);
}

TEST(UidTest, SerializationPreservesAllFields) {
    auto uid1 = uid_t::generate(999, UidFlags::IS_PLAYER | UidFlags::IS_ITEM);
    uint8_t buffer[sizeof(uid_t)];

    uid1.to_bytes(buffer);
    auto result = uid_t::from_bytes(buffer);

    ASSERT_TRUE(result);
    auto uid2 = result.value();

    EXPECT_EQ(uid1.seed_id(), uid2.seed_id());
    EXPECT_EQ(uid1.flags(), uid2.flags());
    EXPECT_EQ(uid1.random_id(), uid2.random_id());
    EXPECT_EQ(uid1.timestamp(), uid2.timestamp());
}

TEST(UidTest, SerializationMultipleRoundTrips) {
    auto uid1 = uid_t::generate(123, UidFlags::IS_ARMOR);
    uint8_t buffer[sizeof(uid_t)];

    // First round trip
    uid1.to_bytes(buffer);
    auto result1 = uid_t::from_bytes(buffer);
    ASSERT_TRUE(result1);

    // Second round trip
    result1.value().to_bytes(buffer);
    auto result2 = uid_t::from_bytes(buffer);
    ASSERT_TRUE(result2);

    EXPECT_EQ(uid1, result2.value());
}

// ============================================================================
// uid_t Integration Tests
// ============================================================================

TEST(UidTest, GenerateMultipleWithDifferentSeeds) {
    auto uid1 = uid_t::generate(1, UidFlags::IS_PLAYER);
    auto uid2 = uid_t::generate(2, UidFlags::IS_PLAYER);
    auto uid3 = uid_t::generate(3, UidFlags::IS_PLAYER);

    EXPECT_NE(uid1, uid2);
    EXPECT_NE(uid2, uid3);
    EXPECT_NE(uid1, uid3);
}

TEST(UidTest, GenerateManyUnique) {
    std::set<uint64_t> random_ids;

    for (int i = 0; i < 1000; ++i) {
        auto uid = uid_t::generate(1, UidFlags::IS_PLAYER);
        random_ids.insert(uid.random_id());
    }

    // All random IDs should be unique
    EXPECT_EQ(random_ids.size(), 1000);
}

TEST(UidTest, ComplexFlagManipulation) {
    uid_t uid;

    // Set multiple flags
    uid.is_item(true);
    uid.is_weapon(true);
    EXPECT_TRUE(uid.is_item());
    EXPECT_TRUE(uid.is_weapon());

    // Clear one flag
    uid.is_weapon(false);
    EXPECT_TRUE(uid.is_item());
    EXPECT_FALSE(uid.is_weapon());

    // Add more flags
    uid.is_armor(true);
    EXPECT_TRUE(uid.is_item());
    EXPECT_TRUE(uid.is_armor());
}

TEST(UidTest, ItemWithWeaponAndArmorFlags) {
    uid_t uid;
    uid.is_item(true);
    uid.is_weapon(true);

    EXPECT_TRUE(uid.is_item());
    EXPECT_TRUE(uid.is_weapon());

    uid_t uid2;
    uid2.is_item(true);
    uid2.is_armor(true);

    EXPECT_TRUE(uid2.is_item());
    EXPECT_TRUE(uid2.is_armor());
}

TEST(UidTest, TimestampPreservation) {
    auto time_result = dungeons::Time::now();
    ASSERT_TRUE(time_result);
    auto original_time = time_result.value();

    auto uid = uid_t::from_repository(100, UidFlags::IS_PLAYER, original_time, 12345);

    EXPECT_EQ(uid.timestamp(), original_time);
}

TEST(UidTest, ZeroSeedIdValid) {
    auto uid = uid_t::generate(0, UidFlags::IS_PLAYER);
    EXPECT_EQ(uid.seed_id(), 0);
}

TEST(UidTest, MaxSeedIdValid) {
    auto uid = uid_t::generate(UINT32_MAX, UidFlags::IS_PLAYER);
    EXPECT_EQ(uid.seed_id(), UINT32_MAX);
}

TEST(UidTest, MaxRandomIdValid) {
    auto time_result = dungeons::Time::now();
    ASSERT_TRUE(time_result);

    auto uid = uid_t::from_repository(1, UidFlags::IS_PLAYER, time_result.value(), UINT64_MAX);
    EXPECT_EQ(uid.random_id(), UINT64_MAX);
}