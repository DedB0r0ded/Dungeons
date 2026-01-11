// GameBackend.h
// TODO: fix newlines
// TODO: translate to russian (comments and error messages)
// TODO: remove redundant private modifiers
// TODO: rename GameBackend to Core
#pragma once

#include "./Repository.h"
#include "./Index.h"
#include "./Player.h"
#include "./Factories.h"
#include "../result.h"
#include <memory>
#include <string>

namespace dungeons::backend {

    class GameBackend {
    private:
        // SQLite connection would go here
        // sqlite3* db_;

        // Repositories
        std::unique_ptr<ArmorMetaRepository> armor_meta_repo_;
        std::unique_ptr<WeaponMetaRepository> weapon_meta_repo_;
        std::unique_ptr<InventoryRepository> inventory_repo_;
        std::unique_ptr<PlayerRepository> player_repo_;
        std::unique_ptr<EnemyMetaRepository> enemy_meta_repo_;
        std::unique_ptr<LocationRepository> location_repo_;
        std::unique_ptr<RoomRepository> room_repo_;

        // Indexes
        std::unique_ptr<ArmorMetaIndex> armor_meta_index_;
        std::unique_ptr<WeaponMetaIndex> weapon_meta_index_;
        std::unique_ptr<EnemyMetaIndex> enemy_meta_index_;
        std::unique_ptr<InventoryIndex> inventory_index_;
        std::unique_ptr<LocationIndex> location_index_;
        std::unique_ptr<RoomIndex> room_index_;
        std::unique_ptr<PlayerIndex> player_index_;

        // Factories
        std::shared_ptr<WeaponFactory> weapon_factory_;
        std::shared_ptr<ArmorFactory> armor_factory_;
        std::shared_ptr<InventoryFactory> inventory_factory_;
        std::shared_ptr<EnemyFactory> enemy_factory_;

        // Current player
        std::shared_ptr<Player> current_player_;

        // Data directory
        std::string data_directory_;

    public:
        // Constructor
        GameBackend() noexcept
            : armor_meta_repo_(nullptr),
            weapon_meta_repo_(nullptr),
            inventory_repo_(nullptr),
            player_repo_(nullptr),
            enemy_meta_repo_(nullptr),
            location_repo_(nullptr),
            room_repo_(nullptr),
            armor_meta_index_(nullptr),
            weapon_meta_index_(nullptr),
            enemy_meta_index_(nullptr),
            inventory_index_(nullptr),
            location_index_(nullptr),
            room_index_(nullptr),
            player_index_(nullptr),
            weapon_factory_(nullptr),
            armor_factory_(nullptr),
            inventory_factory_(nullptr),
            enemy_factory_(nullptr),
            current_player_(nullptr),
            data_directory_("./data") {

            initialize();
        }

        explicit GameBackend(const std::string& data_directory) noexcept
            : armor_meta_repo_(nullptr),
            weapon_meta_repo_(nullptr),
            inventory_repo_(nullptr),
            player_repo_(nullptr),
            enemy_meta_repo_(nullptr),
            location_repo_(nullptr),
            room_repo_(nullptr),
            armor_meta_index_(nullptr),
            weapon_meta_index_(nullptr),
            enemy_meta_index_(nullptr),
            inventory_index_(nullptr),
            location_index_(nullptr),
            room_index_(nullptr),
            player_index_(nullptr),
            weapon_factory_(nullptr),
            armor_factory_(nullptr),
            inventory_factory_(nullptr),
            enemy_factory_(nullptr),
            current_player_(nullptr),
            data_directory_(data_directory) {

            initialize();
        }

        // Delete copy/move
        GameBackend(const GameBackend&) = delete;
        GameBackend(GameBackend&&) = delete;
        GameBackend& operator=(const GameBackend&) = delete;
        GameBackend& operator=(GameBackend&&) = delete;

        // Destructor - saves all data
        ~GameBackend() {
            save_all();
        }

        // Initialize repositories and indexes
        void initialize() noexcept {
            // Create repositories
            armor_meta_repo_ = std::make_unique<ArmorMetaRepository>();
            weapon_meta_repo_ = std::make_unique<WeaponMetaRepository>();
            inventory_repo_ = std::make_unique<InventoryRepository>();
            player_repo_ = std::make_unique<PlayerRepository>();
            enemy_meta_repo_ = std::make_unique<EnemyMetaRepository>();
            location_repo_ = std::make_unique<LocationRepository>();
            room_repo_ = std::make_unique<RoomRepository>();

            // Create indexes
            armor_meta_index_ = std::make_unique<ArmorMetaIndex>();
            weapon_meta_index_ = std::make_unique<WeaponMetaIndex>();
            enemy_meta_index_ = std::make_unique<EnemyMetaIndex>();
            inventory_index_ = std::make_unique<InventoryIndex>();
            location_index_ = std::make_unique<LocationIndex>();
            room_index_ = std::make_unique<RoomIndex>();
            player_index_ = std::make_unique<PlayerIndex>();

            // Load all data from repositories into indexes
            load_all();

            // Create factories
            create_factories();
        }

        // Load all data from repositories
        void load_all() noexcept {
            // This would load data from SQLite repositories into indexes
            // For now, this is a placeholder
        }

        // Save all data to repositories
        void save_all() noexcept {
            // Save all indexes to repositories
            // This would iterate through indexes and save to SQLite
            // For now, this is a placeholder
        }

        // Create factories with meta pools from indexes
        void create_factories() noexcept {
            // Get meta pools from indexes
            auto weapon_meta_vector = weapon_meta_index_->as_vector();
            auto armor_meta_vector = armor_meta_index_->as_vector();
            auto enemy_meta_vector = enemy_meta_index_->as_vector();

            // Create factories
            weapon_factory_ = std::make_shared<WeaponFactory>(weapon_meta_vector);
            armor_factory_ = std::make_shared<ArmorFactory>(armor_meta_vector);
            inventory_factory_ = std::make_shared<InventoryFactory>(
                weapon_factory_, armor_factory_);
            enemy_factory_ = std::make_shared<EnemyFactory>(
                enemy_meta_vector, weapon_factory_, armor_factory_, inventory_factory_);
        }

        // Current player management
        ::dungeons::Result<void> set_current_player(const uid_t& uid) noexcept {
            if (!player_index_) {
                return ::dungeons::Err(
                    ::dungeons::ErrorCode::INVALID_ARGUMENT,
                    "Player index not initialized");
            }

            auto player_result = player_index_->get(uid);
            if (!player_result) {
                return ::dungeons::Err(
                    player_result.error().code(),
                    player_result.error().message());
            }

            current_player_ = player_result.value();
            return ::dungeons::Ok();
        }

        std::shared_ptr<Player> current_player() const noexcept {
            return current_player_;
        }

        // Index getters
        ArmorMetaIndex& armor_meta_index() noexcept { return *armor_meta_index_; }
        const ArmorMetaIndex& armor_meta_index() const noexcept { return *armor_meta_index_; }

        WeaponMetaIndex& weapon_meta_index() noexcept { return *weapon_meta_index_; }
        const WeaponMetaIndex& weapon_meta_index() const noexcept { return *weapon_meta_index_; }

        EnemyMetaIndex& enemy_meta_index() noexcept { return *enemy_meta_index_; }
        const EnemyMetaIndex& enemy_meta_index() const noexcept { return *enemy_meta_index_; }

        InventoryIndex& inventory_index() noexcept { return *inventory_index_; }
        const InventoryIndex& inventory_index() const noexcept { return *inventory_index_; }

        LocationIndex& location_index() noexcept { return *location_index_; }
        const LocationIndex& location_index() const noexcept { return *location_index_; }

        RoomIndex& room_index() noexcept { return *room_index_; }
        const RoomIndex& room_index() const noexcept { return *room_index_; }

        PlayerIndex& player_index() noexcept { return *player_index_; }
        const PlayerIndex& player_index() const noexcept { return *player_index_; }

        // Factory getters
        std::shared_ptr<WeaponFactory> weapon_factory() const noexcept { return weapon_factory_; }
        std::shared_ptr<ArmorFactory> armor_factory() const noexcept { return armor_factory_; }
        std::shared_ptr<InventoryFactory> inventory_factory() const noexcept { return inventory_factory_; }
        std::shared_ptr<EnemyFactory> enemy_factory() const noexcept { return enemy_factory_; }

        // Repository getters (for direct access if needed)
        ArmorMetaRepository& armor_meta_repository() noexcept { return *armor_meta_repo_; }
        WeaponMetaRepository& weapon_meta_repository() noexcept { return *weapon_meta_repo_; }
        InventoryRepository& inventory_repository() noexcept { return *inventory_repo_; }
        PlayerRepository& player_repository() noexcept { return *player_repo_; }
        EnemyMetaRepository& enemy_meta_repository() noexcept { return *enemy_meta_repo_; }
        LocationRepository& location_repository() noexcept { return *location_repo_; }
        RoomRepository& room_repository() noexcept { return *room_repo_; }

        // Validation
        ::dungeons::Result<void> validate() const noexcept {
            if (!armor_meta_index_ || !weapon_meta_index_ || !enemy_meta_index_ ||
                !inventory_index_ || !location_index_ || !room_index_ || !player_index_) {
                return ::dungeons::Err(
                    ::dungeons::ErrorCode::VALIDATION_FAILED,
                    "One or more indexes not initialized");
            }

            if (!armor_meta_repo_ || !weapon_meta_repo_ || !inventory_repo_ ||
                !player_repo_ || !enemy_meta_repo_ || !location_repo_ || !room_repo_) {
                return ::dungeons::Err(
                    ::dungeons::ErrorCode::VALIDATION_FAILED,
                    "One or more repositories not initialized");
            }

            return ::dungeons::Ok();
        }
    };

} // namespace dungeons::backend