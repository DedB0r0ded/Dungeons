// Repository.h
// TODO: fix newlines
// TODO: translate to russian (comments and error messages)
// TODO: remove redundant private modifiers
#pragma once

#include "./Entity.h"
#include "../result.h"
#include <vector>
#include <string>
#include <memory>

// Note: SQLite integration requires sqlite3.h
// For now, this is a template for the interface

namespace dungeons::backend {

    template<typename T>
    class BaseRepository {
        static_assert(std::is_base_of<Entity, T>::value,
            "T must derive from Entity");

    protected:
        std::string table_name_;
        std::string data_directory_;
        // SQLite connection would go here
        // sqlite3* db_;

        BaseRepository(const std::string& table_name,
            const std::string& data_directory = "./data") noexcept
            : table_name_(table_name),
            data_directory_(data_directory) {
        }

    public:
        // Delete copy/move constructors and operators
        BaseRepository() = delete;
        BaseRepository(const BaseRepository&) = delete;
        BaseRepository(BaseRepository&&) = delete;
        BaseRepository& operator=(const BaseRepository&) = delete;
        BaseRepository& operator=(BaseRepository&&) = delete;

        virtual ~BaseRepository() = default;

        // Core interface
        virtual ::dungeons::Result<void> save(const T& entity) noexcept = 0;
        virtual ::dungeons::Result<void> save_all(const std::vector<T>& entities) noexcept = 0;
        virtual ::dungeons::Result<std::vector<T>> load_all() noexcept = 0;

        // Optional: load by UID
        virtual ::dungeons::Result<T> load_by_uid(const uid_t& uid) noexcept {
            return ::dungeons::Err<T>(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "Load by UID not implemented");
        }

        // Optional: delete by UID
        virtual ::dungeons::Result<void> delete_by_uid(const uid_t& uid) noexcept {
            return ::dungeons::Err(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "Delete by UID not implemented");
        }

        // Getters
        const std::string& table_name() const noexcept { return table_name_; }
        const std::string& data_directory() const noexcept { return data_directory_; }
    };

    // Specialized repository classes

    class ArmorMetaRepository : public BaseRepository<ArmorMeta> {
    public:
        ArmorMetaRepository() noexcept
            : BaseRepository<ArmorMeta>("armor_meta", "./data") {}

        ~ArmorMetaRepository() = default;

        ::dungeons::Result<void> save(const ArmorMeta& entity) noexcept override {
            // SQLite implementation would go here
            return ::dungeons::Err(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "ArmorMeta save not yet implemented");
        }

        ::dungeons::Result<void> save_all(const std::vector<ArmorMeta>& entities) noexcept override {
            // SQLite implementation would go here
            return ::dungeons::Err(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "ArmorMeta save_all not yet implemented");
        }

        ::dungeons::Result<std::vector<ArmorMeta>> load_all() noexcept override {
            // SQLite implementation would go here
            return ::dungeons::Err<std::vector<ArmorMeta>>(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "ArmorMeta load_all not yet implemented");
        }
    };

    class WeaponMetaRepository : public BaseRepository<WeaponMeta> {
    public:
        WeaponMetaRepository() noexcept
            : BaseRepository<WeaponMeta>("weapon_meta", "./data") {}

        ~WeaponMetaRepository() = default;

        ::dungeons::Result<void> save(const WeaponMeta& entity) noexcept override {
            return ::dungeons::Err(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "WeaponMeta save not yet implemented");
        }

        ::dungeons::Result<void> save_all(const std::vector<WeaponMeta>& entities) noexcept override {
            return ::dungeons::Err(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "WeaponMeta save_all not yet implemented");
        }

        ::dungeons::Result<std::vector<WeaponMeta>> load_all() noexcept override {
            return ::dungeons::Err<std::vector<WeaponMeta>>(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "WeaponMeta load_all not yet implemented");
        }
    };

    class InventoryRepository : public BaseRepository<Inventory> {
    public:
        InventoryRepository() noexcept
            : BaseRepository<Inventory>("inventory", "./data") {}

        ~InventoryRepository() = default;

        ::dungeons::Result<void> save(const Inventory& entity) noexcept override {
            return ::dungeons::Err(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "Inventory save not yet implemented");
        }

        ::dungeons::Result<void> save_all(const std::vector<Inventory>& entities) noexcept override {
            return ::dungeons::Err(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "Inventory save_all not yet implemented");
        }

        ::dungeons::Result<std::vector<Inventory>> load_all() noexcept override {
            return ::dungeons::Err<std::vector<Inventory>>(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "Inventory load_all not yet implemented");
        }
    };

    class PlayerRepository : public BaseRepository<Player> {
    public:
        PlayerRepository() noexcept
            : BaseRepository<Player>("player", "./data") {}

        ~PlayerRepository() = default;

        ::dungeons::Result<void> save(const Player& entity) noexcept override {
            return ::dungeons::Err(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "Player save not yet implemented");
        }

        ::dungeons::Result<void> save_all(const std::vector<Player>& entities) noexcept override {
            return ::dungeons::Err(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "Player save_all not yet implemented");
        }

        ::dungeons::Result<std::vector<Player>> load_all() noexcept override {
            return ::dungeons::Err<std::vector<Player>>(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "Player load_all not yet implemented");
        }
    };

    class EnemyMetaRepository : public BaseRepository<EnemyMeta> {
    public:
        EnemyMetaRepository() noexcept
            : BaseRepository<EnemyMeta>("enemy_meta", "./data") {}

        ~EnemyMetaRepository() = default;

        ::dungeons::Result<void> save(const EnemyMeta& entity) noexcept override {
            return ::dungeons::Err(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "EnemyMeta save not yet implemented");
        }

        ::dungeons::Result<void> save_all(const std::vector<EnemyMeta>& entities) noexcept override {
            return ::dungeons::Err(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "EnemyMeta save_all not yet implemented");
        }

        ::dungeons::Result<std::vector<EnemyMeta>> load_all() noexcept override {
            return ::dungeons::Err<std::vector<EnemyMeta>>(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "EnemyMeta load_all not yet implemented");
        }
    };

    class LocationRepository : public BaseRepository<Location> {
    public:
        LocationRepository() noexcept
            : BaseRepository<Location>("location", "./data") {}

        ~LocationRepository() = default;

        ::dungeons::Result<void> save(const Location& entity) noexcept override {
            return ::dungeons::Err(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "Location save not yet implemented");
        }

        ::dungeons::Result<void> save_all(const std::vector<Location>& entities) noexcept override {
            return ::dungeons::Err(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "Location save_all not yet implemented");
        }

        ::dungeons::Result<std::vector<Location>> load_all() noexcept override {
            return ::dungeons::Err<std::vector<Location>>(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "Location load_all not yet implemented");
        }
    };

    class RoomRepository : public BaseRepository<Room> {
    public:
        RoomRepository() noexcept
            : BaseRepository<Room>("room", "./data") {}

        ~RoomRepository() = default;

        ::dungeons::Result<void> save(const Room& entity) noexcept override {
            return ::dungeons::Err(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "Room save not yet implemented");
        }

        ::dungeons::Result<void> save_all(const std::vector<Room>& entities) noexcept override {
            return ::dungeons::Err(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "Room save_all not yet implemented");
        }

        ::dungeons::Result<std::vector<Room>> load_all() noexcept override {
            return ::dungeons::Err<std::vector<Room>>(
                ::dungeons::ErrorCode::NOT_IMPLEMENTED,
                "Room load_all not yet implemented");
        }
    };

} // namespace dungeons::backend