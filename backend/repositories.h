// sqlite_repositories.h
#pragma once

#include "./Player.h"
#include "./Enemy.h"
#include "./Location.h"
#include "./Room.h"
#include "./Inventory.h"
#include "./ItemMeta.h"
#include "../Logger.h"
#include "../external/sqlite/sqlite3.h"

#include <string>
#include <memory>
#include <vector>

namespace dungeons::backend {

    //class Room; // Forward declaration

    // --- Minimal repository interfaces (match implementations below) ---
    // These are provided so this translation unit is self-contained.
    // If your project already provides repository interfaces elsewhere,
    // remove these declarations and include the project's header instead.

    class ArmorMetaRepository {
    public:
        virtual ~ArmorMetaRepository() = default;
        virtual ::dungeons::Result<void> save(std::shared_ptr<ArmorMeta> meta) noexcept = 0;
        virtual ::dungeons::Result<void> save_all(const std::vector<std::shared_ptr<ArmorMeta>>& metas) noexcept = 0;
        virtual ::dungeons::Result<std::vector<std::shared_ptr<ArmorMeta>>> load_all() noexcept = 0;
    };

    class WeaponMetaRepository {
    public:
        virtual ~WeaponMetaRepository() = default;
        virtual ::dungeons::Result<void> save(std::shared_ptr<WeaponMeta> meta) noexcept = 0;
        virtual ::dungeons::Result<void> save_all(const std::vector<std::shared_ptr<WeaponMeta>>& metas) noexcept = 0;
        virtual ::dungeons::Result<std::vector<std::shared_ptr<WeaponMeta>>> load_all() noexcept = 0;
    };

    class InventoryRepository {
    public:
        virtual ~InventoryRepository() = default;
        virtual ::dungeons::Result<void> save(std::shared_ptr<Inventory> inventory) noexcept = 0;
        virtual ::dungeons::Result<void> save_all(const std::vector<std::shared_ptr<Inventory>>& inventories) noexcept = 0;
        virtual ::dungeons::Result<std::vector<std::shared_ptr<Inventory>>> load_all() noexcept = 0;
    };

    class PlayerRepository {
    public:
        virtual ~PlayerRepository() = default;
        virtual ::dungeons::Result<void> save(std::shared_ptr<Player> player) noexcept = 0;
        virtual ::dungeons::Result<void> save_all(const std::vector<std::shared_ptr<Player>>& players) noexcept = 0;
        virtual ::dungeons::Result<std::vector<std::shared_ptr<Player>>> load_all() noexcept = 0;
    };

    class EnemyMetaRepository {
    public:
        virtual ~EnemyMetaRepository() = default;
        virtual ::dungeons::Result<void> save(std::shared_ptr<EnemyMeta> meta) noexcept = 0;
        virtual ::dungeons::Result<void> save_all(const std::vector<std::shared_ptr<EnemyMeta>>& metas) noexcept = 0;
        virtual ::dungeons::Result<std::vector<std::shared_ptr<EnemyMeta>>> load_all() noexcept = 0;
    };

    class LocationRepository {
    public:
        virtual ~LocationRepository() = default;
        virtual ::dungeons::Result<void> save(std::shared_ptr<Location> location) noexcept = 0;
        virtual ::dungeons::Result<void> save_all(const std::vector<std::shared_ptr<Location>>& locations) noexcept = 0;
        virtual ::dungeons::Result<std::vector<std::shared_ptr<Location>>> load_all() noexcept = 0;
    };

    class RoomRepository {
    public:
        virtual ~RoomRepository() = default;
        virtual ::dungeons::Result<void> save(std::shared_ptr<Room> room) noexcept = 0;
        virtual ::dungeons::Result<void> save_all(const std::vector<std::shared_ptr<Room>>& rooms) noexcept = 0;
        virtual ::dungeons::Result<std::vector<std::shared_ptr<Room>>> load_all() noexcept = 0;
    };

    // --- SQLite helper base ---
    class SQLiteRepository {
    protected:
        std::string db_path_;
        sqlite3* db_;

        ::dungeons::Result<void> open_connection() noexcept {
            if (db_)
                return ::dungeons::Ok();
            int rc = sqlite3_open(db_path_.c_str(), &db_);
            if (rc != SQLITE_OK) {
                ::dungeons::Logger::instance().error("Не удалось открыть БД: " + std::string(sqlite3_errmsg(db_ ? db_ : nullptr)));
                return ::dungeons::Err(::dungeons::ErrorCode::IO_ERROR, "Не удалось открыть БД");
            }
            return ::dungeons::Ok();
        }

        void close_connection() noexcept {
            if (db_) {
                sqlite3_close(db_);
                db_ = nullptr;
            }
        }

        ::dungeons::Result<void> execute_sql(const std::string& sql) noexcept {
            auto open_result = open_connection();
            if (!open_result)
                return open_result;
            char* err_msg = nullptr;
            int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err_msg);
            if (rc != SQLITE_OK) {
                std::string error = err_msg ? err_msg : "Unknown error";
                if (err_msg) sqlite3_free(err_msg);
                ::dungeons::Logger::instance().error("SQL ошибка: " + error);
                return ::dungeons::Err(::dungeons::ErrorCode::IO_ERROR, "SQL ошибка: " + error);
            }
            return ::dungeons::Ok();
        }

    public:
        SQLiteRepository(const std::string& db_path) noexcept
            : db_path_(db_path), db_(nullptr) {
        }

        virtual ~SQLiteRepository() {
            close_connection();
        }

        SQLiteRepository(const SQLiteRepository&) = delete;
        SQLiteRepository& operator=(const SQLiteRepository&) = delete;
        SQLiteRepository(SQLiteRepository&& other) noexcept
            : db_path_(std::move(other.db_path_)), db_(other.db_) {
            other.db_ = nullptr;
        }
        SQLiteRepository& operator=(SQLiteRepository&& other) noexcept {
            if (this != &other) {
                close_connection();
                db_path_ = std::move(other.db_path_);
                db_ = other.db_;
                other.db_ = nullptr;
            }
            return *this;
        }
    };

    // --- Implementations ---

    // Player
    class PlayerRepositoryImpl : public PlayerRepository, public SQLiteRepository {
    public:
        explicit PlayerRepositoryImpl(const std::string& db_path) noexcept
            : SQLiteRepository(db_path) {
            create_table();
        }

        ::dungeons::Result<void> create_table() noexcept {
            std::string sql = R"(
                CREATE TABLE IF NOT EXISTS players (
                    uid_seed_id INTEGER NOT NULL,
                    uid_flags INTEGER NOT NULL,
                    uid_timestamp_year INTEGER NOT NULL,
                    uid_timestamp_month INTEGER NOT NULL,
                    uid_timestamp_day INTEGER NOT NULL,
                    uid_timestamp_millis INTEGER NOT NULL,
                    uid_timestamp_tz_offset INTEGER NOT NULL,
                    uid_random_id INTEGER NOT NULL,
                    name TEXT NOT NULL,
                    level INTEGER NOT NULL,
                    experience INTEGER NOT NULL,
                    current_health INTEGER NOT NULL,
                    base_max_health INTEGER NOT NULL,
                    base_defense INTEGER NOT NULL,
                    base_damage INTEGER NOT NULL,
                    base_attr_health INTEGER NOT NULL,
                    base_attr_strength INTEGER NOT NULL,
                    inventory_uid_seed_id INTEGER,
                    inventory_uid_random_id INTEGER,
                    current_room_uid_seed_id INTEGER,
                    current_room_uid_random_id INTEGER,
                    PRIMARY KEY (uid_seed_id, uid_random_id)
                );
            )";
            return execute_sql(sql);
        }

        ::dungeons::Result<void> save(std::shared_ptr<Player> player) noexcept override {
            if (!player)
                return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT, "player is null");
            auto open_result = open_connection();
            if (!open_result)
                return open_result;
            std::string sql = R"(
                INSERT OR REPLACE INTO players VALUES (
                    ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?
                );
            )";
            sqlite3_stmt* stmt = nullptr;
            int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                ::dungeons::Logger::instance().error("Не удалось подготовить запрос: " + std::string(sqlite3_errmsg(db_)));
                if (stmt) sqlite3_finalize(stmt);
                return ::dungeons::Err(::dungeons::ErrorCode::IO_ERROR, "Не удалось подготовить запрос");
            }
            // Bind UID
            sqlite3_bind_int(stmt, 1, static_cast<int>(player->uid().seed_id()));
            sqlite3_bind_int(stmt, 2, static_cast<int>(player->uid().flags()));
            sqlite3_bind_int(stmt, 3, static_cast<int>(player->uid().timestamp().year()));
            sqlite3_bind_int(stmt, 4, static_cast<int>(player->uid().timestamp().month()));
            sqlite3_bind_int(stmt, 5, static_cast<int>(player->uid().timestamp().day()));
            // milliseconds_from_midnight stored as 32-bit
            sqlite3_bind_int(stmt, 6, static_cast<int>(player->uid().timestamp().milliseconds_from_midnight()));
            sqlite3_bind_int(stmt, 7, static_cast<int>(player->uid().timestamp().timezone_offset_minutes()));
            // random_id is 64-bit
            sqlite3_bind_int64(stmt, 8, static_cast<sqlite3_int64>(player->uid().random_id()));
            // Player data
            sqlite3_bind_text(stmt, 9, player->name().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 10, player->level());
            sqlite3_bind_int(stmt, 11, player->experience());
            sqlite3_bind_int(stmt, 12, player->current_health());
            sqlite3_bind_int(stmt, 13, player->base_max_health());
            sqlite3_bind_int(stmt, 14, player->base_defense());
            sqlite3_bind_int(stmt, 15, player->base_damage());
            sqlite3_bind_int(stmt, 16, player->base_attributes().health);
            sqlite3_bind_int(stmt, 17, player->base_attributes().strength);
            // Inventory references
            sqlite3_bind_int(stmt, 18, static_cast<int>(player->inventory().uid().seed_id()));
            sqlite3_bind_int64(stmt, 19, static_cast<sqlite3_int64>(player->inventory().uid().random_id()));
            auto room = player->current_room().lock();
            if (room) {
                sqlite3_bind_int(stmt, 20, static_cast<int>(room->uid().seed_id()));
                sqlite3_bind_int64(stmt, 21, static_cast<sqlite3_int64>(room->uid().random_id()));
            }
            else {
                sqlite3_bind_null(stmt, 20);
                sqlite3_bind_null(stmt, 21);
            }
            rc = sqlite3_step(stmt);
            sqlite3_finalize(stmt);
            if (rc != SQLITE_DONE) {
                ::dungeons::Logger::instance().error("Не удалось сохранить игрока: " + std::string(sqlite3_errmsg(db_)));
                return ::dungeons::Err(::dungeons::ErrorCode::IO_ERROR, "Не удалось сохранить игрока");
            }
            ::dungeons::Logger::instance().info("Игрок '" + player->name() + "' сохранен");
            return ::dungeons::Ok();
        }

        ::dungeons::Result<void> save_all(const std::vector<std::shared_ptr<Player>>& players) noexcept override {
            for (const auto& p : players) {
                auto r = save(p);
                if (!r) return r;
            }
            return ::dungeons::Ok();
        }

        ::dungeons::Result<std::vector<std::shared_ptr<Player>>> load_all() noexcept override {
            auto open_result = open_connection();
            if (!open_result)
                return ::dungeons::Err<std::vector<std::shared_ptr<Player>>>(open_result.error().code(), open_result.error().message());
            std::string sql = "SELECT * FROM players;";
            sqlite3_stmt* stmt = nullptr;
            int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                ::dungeons::Logger::instance().error("Не удалось подготовить запрос: " + std::string(sqlite3_errmsg(db_)));
                if (stmt) sqlite3_finalize(stmt);
                return ::dungeons::Err<std::vector<std::shared_ptr<Player>>>(::dungeons::ErrorCode::IO_ERROR, "Не удалось подготовить запрос");
            }
            std::vector<std::shared_ptr<Player>> players;
            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                uint32_t seed_id = static_cast<uint32_t>(sqlite3_column_int(stmt, 0));
                UidFlags flags = static_cast<UidFlags>(sqlite3_column_int(stmt, 1));
                ::dungeons::Time timestamp;
                // Setters used for fields (ignore return; validated elsewhere)
                timestamp.year(static_cast<uint16_t>(sqlite3_column_int(stmt, 2)));
                timestamp.month(static_cast<uint8_t>(sqlite3_column_int(stmt, 3)));
                timestamp.day(static_cast<uint8_t>(sqlite3_column_int(stmt, 4)));
                // milliseconds stored as 32-bit
                timestamp.milliseconds_from_midnight(static_cast<uint32_t>(sqlite3_column_int(stmt, 5)));
                timestamp.timezone_offset_minutes(static_cast<int16_t>(sqlite3_column_int(stmt, 6)));
                uint64_t random_id = static_cast<uint64_t>(sqlite3_column_int64(stmt, 7));
                auto uid = uid_t::from_repository(seed_id, flags, timestamp, random_id);
                const unsigned char* txt = sqlite3_column_text(stmt, 8);
                std::string name = txt ? reinterpret_cast<const char*>(txt) : std::string{};
                int32_t level = sqlite3_column_int(stmt, 9);
                int32_t experience = sqlite3_column_int(stmt, 10);
                int32_t current_health = sqlite3_column_int(stmt, 11);
                int32_t base_max_health = sqlite3_column_int(stmt, 12);
                int32_t base_defense = sqlite3_column_int(stmt, 13);
                int32_t base_damage = sqlite3_column_int(stmt, 14);
                Attributes base_attrs(sqlite3_column_int(stmt, 15), sqlite3_column_int(stmt, 16));
                auto player = std::make_shared<Player>(uid, name, level, current_health, base_max_health, base_defense, base_damage, base_attrs);
                players.push_back(player);
            }
            sqlite3_finalize(stmt);
            ::dungeons::Logger::instance().info("Загружено игроков: " + std::to_string(players.size()));
            return ::dungeons::Ok(players);
        }
    };

    // Inventory
    class InventoryRepositoryImpl : public InventoryRepository, public SQLiteRepository {
    public:
        explicit InventoryRepositoryImpl(const std::string& db_path) noexcept
            : SQLiteRepository(db_path) {
            create_table();
        }

        ::dungeons::Result<void> create_table() noexcept {
            std::string sql = R"(
                CREATE TABLE IF NOT EXISTS inventories (
                    uid_seed_id INTEGER NOT NULL,
                    uid_flags INTEGER NOT NULL,
                    uid_timestamp_year INTEGER NOT NULL,
                    uid_timestamp_month INTEGER NOT NULL,
                    uid_timestamp_day INTEGER NOT NULL,
                    uid_timestamp_millis INTEGER NOT NULL,
                    uid_timestamp_tz_offset INTEGER NOT NULL,
                    uid_random_id INTEGER NOT NULL,
                    max_size INTEGER NOT NULL,
                    money_amount INTEGER NOT NULL,
                    PRIMARY KEY (uid_seed_id, uid_random_id)
                );
                CREATE TABLE IF NOT EXISTS inventory_items (
                    inventory_uid_seed_id INTEGER NOT NULL,
                    inventory_uid_random_id INTEGER NOT NULL,
                    slot_index INTEGER NOT NULL,
                    item_uid_seed_id INTEGER NOT NULL,
                    item_uid_random_id INTEGER NOT NULL,
                    FOREIGN KEY (inventory_uid_seed_id, inventory_uid_random_id)
                        REFERENCES inventories(uid_seed_id, uid_random_id)
                );
            )";
            return execute_sql(sql);
        }

        ::dungeons::Result<void> save(std::shared_ptr<Inventory> inventory) noexcept override {
            if (!inventory)
                return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT, "inventory is null");
            auto open_result = open_connection();
            if (!open_result) return open_result;
            // Save inventory main row
            std::string sql = R"(
                INSERT OR REPLACE INTO inventories VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
            )";
            sqlite3_stmt* stmt = nullptr;
            int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                if (stmt) sqlite3_finalize(stmt);
                return ::dungeons::Err(::dungeons::ErrorCode::IO_ERROR, "Не удалось подготовить запрос");
            }
            sqlite3_bind_int(stmt, 1, static_cast<int>(inventory->uid().seed_id()));
            sqlite3_bind_int(stmt, 2, static_cast<int>(inventory->uid().flags()));
            sqlite3_bind_int(stmt, 3, static_cast<int>(inventory->uid().timestamp().year()));
            sqlite3_bind_int(stmt, 4, static_cast<int>(inventory->uid().timestamp().month()));
            sqlite3_bind_int(stmt, 5, static_cast<int>(inventory->uid().timestamp().day()));
            // milliseconds stored as 32-bit
            sqlite3_bind_int(stmt, 6, static_cast<int>(inventory->uid().timestamp().milliseconds_from_midnight()));
            sqlite3_bind_int(stmt, 7, static_cast<int>(inventory->uid().timestamp().timezone_offset_minutes()));
            sqlite3_bind_int64(stmt, 8, static_cast<sqlite3_int64>(inventory->uid().random_id()));
            sqlite3_bind_int(stmt, 9, static_cast<int>(inventory->max_size()));
            sqlite3_bind_int64(stmt, 10, static_cast<sqlite3_int64>(inventory->money().amount()));
            rc = sqlite3_step(stmt);
            sqlite3_finalize(stmt);
            if (rc != SQLITE_DONE) {
                return ::dungeons::Err(::dungeons::ErrorCode::IO_ERROR, "Не удалось сохранить инвентарь");
            }
            // Delete old items
            sql = "DELETE FROM inventory_items WHERE inventory_uid_seed_id = ? AND inventory_uid_random_id = ?;";
            rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                if (stmt) sqlite3_finalize(stmt);
                return ::dungeons::Err(::dungeons::ErrorCode::IO_ERROR, "Не удалось подготовить запрос");
            }
            sqlite3_bind_int(stmt, 1, static_cast<int>(inventory->uid().seed_id()));
            sqlite3_bind_int64(stmt, 2, static_cast<sqlite3_int64>(inventory->uid().random_id()));
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
            // Insert items
            for (size_t i = 0; i < inventory->max_size(); ++i) {
                auto item_result = inventory->get_item(i);
                if (item_result && item_result.value()) {
                    sql = "INSERT INTO inventory_items VALUES (?, ?, ?, ?, ?);";
                    rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
                    if (rc != SQLITE_OK) {
                        if (stmt) sqlite3_finalize(stmt);
                        return ::dungeons::Err(::dungeons::ErrorCode::IO_ERROR, "Не удалось подготовить запрос");
                    }
                    sqlite3_bind_int(stmt, 1, static_cast<int>(inventory->uid().seed_id()));
                    sqlite3_bind_int64(stmt, 2, static_cast<sqlite3_int64>(inventory->uid().random_id()));
                    sqlite3_bind_int(stmt, 3, static_cast<int>(i));
                    // uid() returns Result<uid_t> - use value()
                    sqlite3_bind_int(stmt, 4, static_cast<int>(item_result.value()->uid().value().seed_id()));
                    sqlite3_bind_int64(stmt, 5, static_cast<sqlite3_int64>(item_result.value()->uid().value().random_id()));
                    sqlite3_step(stmt);
                    sqlite3_finalize(stmt);
                }
            }
            ::dungeons::Logger::instance().info("Инвентарь сохранен");
            return ::dungeons::Ok();
        }

        ::dungeons::Result<void> save_all(const std::vector<std::shared_ptr<Inventory>>& inventories) noexcept override {
            for (const auto& inv : inventories) {
                auto r = save(inv);
                if (!r) return r;
            }
            return ::dungeons::Ok();
        }

        ::dungeons::Result<std::vector<std::shared_ptr<Inventory>>> load_all() noexcept override {
            auto open_result = open_connection();
            if (!open_result)
                return ::dungeons::Err<std::vector<std::shared_ptr<Inventory>>>(open_result.error().code(), open_result.error().message());
            std::string sql = "SELECT * FROM inventories;";
            sqlite3_stmt* stmt = nullptr;
            int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                if (stmt) sqlite3_finalize(stmt);
                return ::dungeons::Err<std::vector<std::shared_ptr<Inventory>>>(::dungeons::ErrorCode::IO_ERROR, "Не удалось подготовить запрос");
            }
            std::vector<std::shared_ptr<Inventory>> inventories;
            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                uint32_t seed_id = static_cast<uint32_t>(sqlite3_column_int(stmt, 0));
                UidFlags flags = static_cast<UidFlags>(sqlite3_column_int(stmt, 1));
                ::dungeons::Time timestamp;
                timestamp.year(static_cast<uint16_t>(sqlite3_column_int(stmt, 2)));
                timestamp.month(static_cast<uint8_t>(sqlite3_column_int(stmt, 3)));
                timestamp.day(static_cast<uint8_t>(sqlite3_column_int(stmt, 4)));
                // read milliseconds as 32-bit
                timestamp.milliseconds_from_midnight(static_cast<uint32_t>(sqlite3_column_int(stmt, 5)));
                timestamp.timezone_offset_minutes(static_cast<int16_t>(sqlite3_column_int(stmt, 6)));
                uint64_t random_id = static_cast<uint64_t>(sqlite3_column_int64(stmt, 7));
                auto uid = uid_t::from_repository(seed_id, flags, timestamp, random_id);
                size_t max_size = static_cast<size_t>(sqlite3_column_int(stmt, 8));
                int64_t money_amount = static_cast<int64_t>(sqlite3_column_int64(stmt, 9));
                auto inventory = std::make_shared<Inventory>(uid, max_size, Money(money_amount));
                inventories.push_back(inventory);
            }
            sqlite3_finalize(stmt);
            ::dungeons::Logger::instance().info("Загружено инвентарей: " + std::to_string(inventories.size()));
            return ::dungeons::Ok(inventories);
        }
    };

    // WeaponMeta
    class WeaponMetaRepositoryImpl : public WeaponMetaRepository, public SQLiteRepository {
    public:
        explicit WeaponMetaRepositoryImpl(const std::string& db_path) noexcept
            : SQLiteRepository(db_path) {
            create_table();
        }

        ::dungeons::Result<void> create_table() noexcept {
            std::string sql = R"(
                CREATE TABLE IF NOT EXISTS weapon_meta (
                    uid_seed_id INTEGER NOT NULL,
                    uid_flags INTEGER NOT NULL,
                    uid_timestamp_year INTEGER NOT NULL,
                    uid_timestamp_month INTEGER NOT NULL,
                    uid_timestamp_day INTEGER NOT NULL,
                    uid_timestamp_millis INTEGER NOT NULL,
                    uid_timestamp_tz_offset INTEGER NOT NULL,
                    uid_random_id INTEGER NOT NULL,
                    name TEXT NOT NULL,
                    description TEXT NOT NULL,
                    gen_basis_health INTEGER NOT NULL,
                    gen_basis_strength INTEGER NOT NULL,
                    attack_basis REAL NOT NULL,
                    PRIMARY KEY (uid_seed_id, uid_random_id)
                );
            )";
            return execute_sql(sql);
        }

        ::dungeons::Result<void> save(std::shared_ptr<WeaponMeta> meta) noexcept override {
            if (!meta) return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT, "meta is null");
            auto open_result = open_connection();
            if (!open_result) return open_result;
            std::string sql = "INSERT OR REPLACE INTO weapon_meta VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
            sqlite3_stmt* stmt = nullptr;
            int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                if (stmt) sqlite3_finalize(stmt);
                return ::dungeons::Err(::dungeons::ErrorCode::IO_ERROR, "Не удалось подготовить запрос");
            }
            sqlite3_bind_int(stmt, 1, static_cast<int>(meta->uid().seed_id()));
            sqlite3_bind_int(stmt, 2, static_cast<int>(meta->uid().flags()));
            sqlite3_bind_int(stmt, 3, static_cast<int>(meta->uid().timestamp().year()));
            sqlite3_bind_int(stmt, 4, static_cast<int>(meta->uid().timestamp().month()));
            sqlite3_bind_int(stmt, 5, static_cast<int>(meta->uid().timestamp().day()));
            // milliseconds as 32-bit
            sqlite3_bind_int(stmt, 6, static_cast<int>(meta->uid().timestamp().milliseconds_from_midnight()));
            sqlite3_bind_int(stmt, 7, static_cast<int>(meta->uid().timestamp().timezone_offset_minutes()));
            sqlite3_bind_int64(stmt, 8, static_cast<sqlite3_int64>(meta->uid().random_id()));
            sqlite3_bind_text(stmt, 9, meta->name().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 10, meta->description().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 11, meta->generation_basis().health);
            sqlite3_bind_int(stmt, 12, meta->generation_basis().strength);
            sqlite3_bind_double(stmt, 13, static_cast<double>(meta->attack_basis()));
            rc = sqlite3_step(stmt);
            sqlite3_finalize(stmt);
            if (rc != SQLITE_DONE)
                return ::dungeons::Err(::dungeons::ErrorCode::IO_ERROR, "Не удалось сохранить метаданные оружия");
            ::dungeons::Logger::instance().info("WeaponMeta '" + meta->name() + "' сохранен");
            return ::dungeons::Ok();
        }

        ::dungeons::Result<void> save_all(const std::vector<std::shared_ptr<WeaponMeta>>& metas) noexcept override {
            for (const auto& m : metas) {
                auto r = save(m);
                if (!r) return r;
            }
            return ::dungeons::Ok();
        }

        ::dungeons::Result<std::vector<std::shared_ptr<WeaponMeta>>> load_all() noexcept override {
            auto open_result = open_connection();
            if (!open_result) return ::dungeons::Err<std::vector<std::shared_ptr<WeaponMeta>>>(open_result.error().code(), open_result.error().message());
            std::string sql = "SELECT * FROM weapon_meta;";
            sqlite3_stmt* stmt = nullptr;
            int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                if (stmt) sqlite3_finalize(stmt);
                return ::dungeons::Err<std::vector<std::shared_ptr<WeaponMeta>>>(::dungeons::ErrorCode::IO_ERROR, "Не удалось подготовить запрос");
            }
            std::vector<std::shared_ptr<WeaponMeta>> metas;
            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                uint32_t seed_id = static_cast<uint32_t>(sqlite3_column_int(stmt, 0));
                UidFlags flags = static_cast<UidFlags>(sqlite3_column_int(stmt, 1));
                ::dungeons::Time timestamp;
                timestamp.year(static_cast<uint16_t>(sqlite3_column_int(stmt, 2)));
                timestamp.month(static_cast<uint8_t>(sqlite3_column_int(stmt, 3)));
                timestamp.day(static_cast<uint8_t>(sqlite3_column_int(stmt, 4)));
                timestamp.milliseconds_from_midnight(static_cast<uint32_t>(sqlite3_column_int(stmt, 5)));
                timestamp.timezone_offset_minutes(static_cast<int16_t>(sqlite3_column_int(stmt, 6)));
                uint64_t random_id = static_cast<uint64_t>(sqlite3_column_int64(stmt, 7));
                auto uid = uid_t::from_repository(seed_id, flags, timestamp, random_id);
                const unsigned char* tname = sqlite3_column_text(stmt, 8);
                const unsigned char* tdesc = sqlite3_column_text(stmt, 9);
                std::string name = tname ? reinterpret_cast<const char*>(tname) : std::string{};
                std::string description = tdesc ? reinterpret_cast<const char*>(tdesc) : std::string{};
                Attributes gen_basis(sqlite3_column_int(stmt, 10), sqlite3_column_int(stmt, 11));
                float attack_basis = static_cast<float>(sqlite3_column_double(stmt, 12));
                auto meta = std::make_shared<WeaponMeta>(uid, name, description, gen_basis, attack_basis);
                metas.push_back(meta);
            }
            sqlite3_finalize(stmt);
            ::dungeons::Logger::instance().info("Загружено WeaponMeta: " + std::to_string(metas.size()));
            return ::dungeons::Ok(metas);
        }
    };

    // ArmorMeta
    class ArmorMetaRepositoryImpl : public ArmorMetaRepository, public SQLiteRepository {
    public:
        explicit ArmorMetaRepositoryImpl(const std::string& db_path) noexcept
            : SQLiteRepository(db_path) {
            create_table();
        }

        ::dungeons::Result<void> create_table() noexcept {
            std::string sql = R"(
                CREATE TABLE IF NOT EXISTS armor_meta (
                    uid_seed_id INTEGER NOT NULL,
                    uid_flags INTEGER NOT NULL,
                    uid_timestamp_year INTEGER NOT NULL,
                    uid_timestamp_month INTEGER NOT NULL,
                    uid_timestamp_day INTEGER NOT NULL,
                    uid_timestamp_millis INTEGER NOT NULL,
                    uid_timestamp_tz_offset INTEGER NOT NULL,
                    uid_random_id INTEGER NOT NULL,
                    name TEXT NOT NULL,
                    description TEXT NOT NULL,
                    gen_basis_health INTEGER NOT NULL,
                    gen_basis_strength INTEGER NOT NULL,
                    defense_basis REAL NOT NULL,
                    PRIMARY KEY (uid_seed_id, uid_random_id)
                );
            )";
            return execute_sql(sql);
        }

        ::dungeons::Result<void> save(std::shared_ptr<ArmorMeta> meta) noexcept override {
            if (!meta) return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT, "meta is null");
            auto open_result = open_connection();
            if (!open_result) return open_result;
            std::string sql = "INSERT OR REPLACE INTO armor_meta VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
            sqlite3_stmt* stmt = nullptr;
            int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                if (stmt) sqlite3_finalize(stmt);
                return ::dungeons::Err(::dungeons::ErrorCode::IO_ERROR, "Не удалось подготовить запрос");
            }
            sqlite3_bind_int(stmt, 1, static_cast<int>(meta->uid().seed_id()));
            sqlite3_bind_int(stmt, 2, static_cast<int>(meta->uid().flags()));
            sqlite3_bind_int(stmt, 3, static_cast<int>(meta->uid().timestamp().year()));
            sqlite3_bind_int(stmt, 4, static_cast<int>(meta->uid().timestamp().month()));
            sqlite3_bind_int(stmt, 5, static_cast<int>(meta->uid().timestamp().day()));
            sqlite3_bind_int(stmt, 6, static_cast<int>(meta->uid().timestamp().milliseconds_from_midnight()));
            sqlite3_bind_int(stmt, 7, static_cast<int>(meta->uid().timestamp().timezone_offset_minutes()));
            sqlite3_bind_int64(stmt, 8, static_cast<sqlite3_int64>(meta->uid().random_id()));
            sqlite3_bind_text(stmt, 9, meta->name().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 10, meta->description().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 11, meta->generation_basis().health);
            sqlite3_bind_int(stmt, 12, meta->generation_basis().strength);
            sqlite3_bind_double(stmt, 13, static_cast<double>(meta->defense_basis()));
            rc = sqlite3_step(stmt);
            sqlite3_finalize(stmt);
            if (rc != SQLITE_DONE)
                return ::dungeons::Err(::dungeons::ErrorCode::IO_ERROR, "Не удалось сохранить метаданные брони");
            ::dungeons::Logger::instance().info("ArmorMeta '" + meta->name() + "' сохранен");
            return ::dungeons::Ok();
        }

        ::dungeons::Result<void> save_all(const std::vector<std::shared_ptr<ArmorMeta>>& metas) noexcept override {
            for (const auto& m : metas) {
                auto r = save(m);
                if (!r) return r;
            }
            return ::dungeons::Ok();
        }

        ::dungeons::Result<std::vector<std::shared_ptr<ArmorMeta>>> load_all() noexcept override {
            auto open_result = open_connection();
            if (!open_result) return ::dungeons::Err<std::vector<std::shared_ptr<ArmorMeta>>>(open_result.error().code(), open_result.error().message());
            std::string sql = "SELECT * FROM armor_meta;";
            sqlite3_stmt* stmt = nullptr;
            int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                if (stmt) sqlite3_finalize(stmt);
                return ::dungeons::Err<std::vector<std::shared_ptr<ArmorMeta>>>(::dungeons::ErrorCode::IO_ERROR, "Не удалось подготовить запрос");
            }
            std::vector<std::shared_ptr<ArmorMeta>> metas;
            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                uint32_t seed_id = static_cast<uint32_t>(sqlite3_column_int(stmt, 0));
                UidFlags flags = static_cast<UidFlags>(sqlite3_column_int(stmt, 1));
                ::dungeons::Time timestamp;
                timestamp.year(static_cast<uint16_t>(sqlite3_column_int(stmt, 2)));
                timestamp.month(static_cast<uint8_t>(sqlite3_column_int(stmt, 3)));
                timestamp.day(static_cast<uint8_t>(sqlite3_column_int(stmt, 4)));
                timestamp.milliseconds_from_midnight(static_cast<uint32_t>(sqlite3_column_int(stmt, 5)));
                timestamp.timezone_offset_minutes(static_cast<int16_t>(sqlite3_column_int(stmt, 6)));
                uint64_t random_id = static_cast<uint64_t>(sqlite3_column_int64(stmt, 7));
                auto uid = uid_t::from_repository(seed_id, flags, timestamp, random_id);
                const unsigned char* tname = sqlite3_column_text(stmt, 8);
                const unsigned char* tdesc = sqlite3_column_text(stmt, 9);
                std::string name = tname ? reinterpret_cast<const char*>(tname) : std::string{};
                std::string description = tdesc ? reinterpret_cast<const char*>(tdesc) : std::string{};
                Attributes gen_basis(sqlite3_column_int(stmt, 10), sqlite3_column_int(stmt, 11));
                float defense_basis = static_cast<float>(sqlite3_column_double(stmt, 12));
                auto meta = std::make_shared<ArmorMeta>(uid, name, description, gen_basis, defense_basis);
                metas.push_back(meta);
            }
            sqlite3_finalize(stmt);
            ::dungeons::Logger::instance().info("Загружено ArmorMeta: " + std::to_string(metas.size()));
            return ::dungeons::Ok(metas);
        }
    };

    // EnemyMeta
    class EnemyMetaRepositoryImpl : public EnemyMetaRepository, public SQLiteRepository {
    public:
        explicit EnemyMetaRepositoryImpl(const std::string& db_path) noexcept
            : SQLiteRepository(db_path) {
            create_table();
        }

        ::dungeons::Result<void> create_table() noexcept {
            std::string sql = R"(
                CREATE TABLE IF NOT EXISTS enemy_meta (
                    uid_seed_id INTEGER NOT NULL,
                    uid_flags INTEGER NOT NULL,
                    uid_timestamp_year INTEGER NOT NULL,
                    uid_timestamp_month INTEGER NOT NULL,
                    uid_timestamp_day INTEGER NOT NULL,
                    uid_timestamp_millis INTEGER NOT NULL,
                    uid_timestamp_tz_offset INTEGER NOT NULL,
                    uid_random_id INTEGER NOT NULL,
                    name TEXT NOT NULL,
                    generation_basis REAL NOT NULL,
                    PRIMARY KEY (uid_seed_id, uid_random_id)
                );
            )";
            return execute_sql(sql);
        }

        ::dungeons::Result<void> save(std::shared_ptr<EnemyMeta> meta) noexcept override {
            if (!meta) return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT, "meta is null");
            auto open_result = open_connection();
            if (!open_result) return open_result;
            std::string sql = "INSERT OR REPLACE INTO enemy_meta VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
            sqlite3_stmt* stmt = nullptr;
            int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                if (stmt) sqlite3_finalize(stmt);
                return ::dungeons::Err(::dungeons::ErrorCode::IO_ERROR, "Не удалось подготовить запрос");
            }
            sqlite3_bind_int(stmt, 1, static_cast<int>(meta->uid().seed_id()));
            sqlite3_bind_int(stmt, 2, static_cast<int>(meta->uid().flags()));
            sqlite3_bind_int(stmt, 3, static_cast<int>(meta->uid().timestamp().year()));
            sqlite3_bind_int(stmt, 4, static_cast<int>(meta->uid().timestamp().month()));
            sqlite3_bind_int(stmt, 5, static_cast<int>(meta->uid().timestamp().day()));
            sqlite3_bind_int(stmt, 6, static_cast<int>(meta->uid().timestamp().milliseconds_from_midnight()));
            sqlite3_bind_int(stmt, 7, static_cast<int>(meta->uid().timestamp().timezone_offset_minutes()));
            sqlite3_bind_int64(stmt, 8, static_cast<sqlite3_int64>(meta->uid().random_id()));
            sqlite3_bind_text(stmt, 9, meta->name().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(stmt, 10, static_cast<double>(meta->generation_basis()));
            rc = sqlite3_step(stmt);
            sqlite3_finalize(stmt);
            if (rc != SQLITE_DONE)
                return ::dungeons::Err(::dungeons::ErrorCode::IO_ERROR, "Не удалось сохранить метаданные врага");
            ::dungeons::Logger::instance().info("EnemyMeta '" + meta->name() + "' сохранен");
            return ::dungeons::Ok();
        }

        ::dungeons::Result<void> save_all(const std::vector<std::shared_ptr<EnemyMeta>>& metas) noexcept override {
            for (const auto& m : metas) {
                auto r = save(m);
                if (!r) return r;
            }
            return ::dungeons::Ok();
        }

        ::dungeons::Result<std::vector<std::shared_ptr<EnemyMeta>>> load_all() noexcept override {
            auto open_result = open_connection();
            if (!open_result) return ::dungeons::Err<std::vector<std::shared_ptr<EnemyMeta>>>(open_result.error().code(), open_result.error().message());
            std::string sql = "SELECT * FROM enemy_meta;";
            sqlite3_stmt* stmt = nullptr;
            int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                if (stmt) sqlite3_finalize(stmt);
                return ::dungeons::Err<std::vector<std::shared_ptr<EnemyMeta>>>(::dungeons::ErrorCode::IO_ERROR, "Не удалось подготовить запрос");
            }
            std::vector<std::shared_ptr<EnemyMeta>> metas;
            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                uint32_t seed_id = static_cast<uint32_t>(sqlite3_column_int(stmt, 0));
                UidFlags flags = static_cast<UidFlags>(sqlite3_column_int(stmt, 1));
                ::dungeons::Time timestamp;
                timestamp.year(static_cast<uint16_t>(sqlite3_column_int(stmt, 2)));
                timestamp.month(static_cast<uint8_t>(sqlite3_column_int(stmt, 3)));
                timestamp.day(static_cast<uint8_t>(sqlite3_column_int(stmt, 4)));
                timestamp.milliseconds_from_midnight(static_cast<uint32_t>(sqlite3_column_int(stmt, 5)));
                timestamp.timezone_offset_minutes(static_cast<int16_t>(sqlite3_column_int(stmt, 6)));
                uint64_t random_id = static_cast<uint64_t>(sqlite3_column_int64(stmt, 7));
                auto uid = uid_t::from_repository(seed_id, flags, timestamp, random_id);
                const unsigned char* tname = sqlite3_column_text(stmt, 8);
                std::string name = tname ? reinterpret_cast<const char*>(tname) : std::string{};
                float generation_basis = static_cast<float>(sqlite3_column_double(stmt, 9));
                auto meta = std::make_shared<EnemyMeta>(uid, name, generation_basis);
                metas.push_back(meta);
            }
            sqlite3_finalize(stmt);
            ::dungeons::Logger::instance().info("Загружено EnemyMeta: " + std::to_string(metas.size()));
            return ::dungeons::Ok(metas);
        }
    };

    // Location
    class LocationRepositoryImpl : public LocationRepository, public SQLiteRepository {
    public:
        explicit LocationRepositoryImpl(const std::string& db_path) noexcept
            : SQLiteRepository(db_path) {
            create_table();
        }

        ::dungeons::Result<void> create_table() noexcept {
            std::string sql = R"(
                CREATE TABLE IF NOT EXISTS locations (
                    uid_seed_id INTEGER NOT NULL,
                    uid_flags INTEGER NOT NULL,
                    uid_timestamp_year INTEGER NOT NULL,
                    uid_timestamp_month INTEGER NOT NULL,
                    uid_timestamp_day INTEGER NOT NULL,
                    uid_timestamp_millis INTEGER NOT NULL,
                    uid_timestamp_tz_offset INTEGER NOT NULL,
                    uid_random_id INTEGER NOT NULL,
                    name TEXT NOT NULL,
                    description TEXT NOT NULL,
                    PRIMARY KEY (uid_seed_id, uid_random_id)
                );
            )";
            return execute_sql(sql);
        }

        ::dungeons::Result<void> save(std::shared_ptr<Location> location) noexcept override {
            if (!location) return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT, "location is null");
            auto open_result = open_connection();
            if (!open_result) return open_result;
            std::string sql = "INSERT OR REPLACE INTO locations VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
            sqlite3_stmt* stmt = nullptr;
            int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                if (stmt) sqlite3_finalize(stmt);
                return ::dungeons::Err(::dungeons::ErrorCode::IO_ERROR, "Не удалось подготовить запрос");
            }
            sqlite3_bind_int(stmt, 1, static_cast<int>(location->uid().seed_id()));
            sqlite3_bind_int(stmt, 2, static_cast<int>(location->uid().flags()));
            sqlite3_bind_int(stmt, 3, static_cast<int>(location->uid().timestamp().year()));
            sqlite3_bind_int(stmt, 4, static_cast<int>(location->uid().timestamp().month()));
            sqlite3_bind_int(stmt, 5, static_cast<int>(location->uid().timestamp().day()));
            sqlite3_bind_int(stmt, 6, static_cast<int>(location->uid().timestamp().milliseconds_from_midnight()));
            sqlite3_bind_int(stmt, 7, static_cast<int>(location->uid().timestamp().timezone_offset_minutes()));
            sqlite3_bind_int64(stmt, 8, static_cast<sqlite3_int64>(location->uid().random_id()));
            sqlite3_bind_text(stmt, 9, location->name().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 10, location->description().c_str(), -1, SQLITE_TRANSIENT);
            rc = sqlite3_step(stmt);
            sqlite3_finalize(stmt);
            if (rc != SQLITE_DONE)
                return ::dungeons::Err(::dungeons::ErrorCode::IO_ERROR, "Не удалось сохранить локацию");
            ::dungeons::Logger::instance().info("Location '" + location->name() + "' сохранена");
            return ::dungeons::Ok();
        }

        ::dungeons::Result<void> save_all(const std::vector<std::shared_ptr<Location>>& locations) noexcept override {
            for (const auto& loc : locations) {
                auto r = save(loc);
                if (!r) return r;
            }
            return ::dungeons::Ok();
        }

        ::dungeons::Result<std::vector<std::shared_ptr<Location>>> load_all() noexcept override {
            auto open_result = open_connection();
            if (!open_result) return ::dungeons::Err<std::vector<std::shared_ptr<Location>>>(open_result.error().code(), open_result.error().message());
            std::string sql = "SELECT * FROM locations;";
            sqlite3_stmt* stmt = nullptr;
            int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                if (stmt) sqlite3_finalize(stmt);
                return ::dungeons::Err<std::vector<std::shared_ptr<Location>>>(::dungeons::ErrorCode::IO_ERROR, "Не удалось подготовить запрос");
            }
            std::vector<std::shared_ptr<Location>> locations;
            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                uint32_t seed_id = static_cast<uint32_t>(sqlite3_column_int(stmt, 0));
                UidFlags flags = static_cast<UidFlags>(sqlite3_column_int(stmt, 1));
                ::dungeons::Time timestamp;
                timestamp.year(static_cast<uint16_t>(sqlite3_column_int(stmt, 2)));
                timestamp.month(static_cast<uint8_t>(sqlite3_column_int(stmt, 3)));
                timestamp.day(static_cast<uint8_t>(sqlite3_column_int(stmt, 4)));
                timestamp.milliseconds_from_midnight(static_cast<uint32_t>(sqlite3_column_int(stmt, 5)));
                timestamp.timezone_offset_minutes(static_cast<int16_t>(sqlite3_column_int(stmt, 6)));
                uint64_t random_id = static_cast<uint64_t>(sqlite3_column_int64(stmt, 7));
                auto uid = uid_t::from_repository(seed_id, flags, timestamp, random_id);
                const unsigned char* tname = sqlite3_column_text(stmt, 8);
                const unsigned char* tdesc = sqlite3_column_text(stmt, 9);
                std::string name = tname ? reinterpret_cast<const char*>(tname) : std::string{};
                std::string description = tdesc ? reinterpret_cast<const char*>(tdesc) : std::string{};
                auto location = std::make_shared<Location>(uid, name, description);
                locations.push_back(location);
            }
            sqlite3_finalize(stmt);
            ::dungeons::Logger::instance().info("Загружено Location: " + std::to_string(locations.size()));
            return ::dungeons::Ok(locations);
        }
    };

    // Room
    class RoomRepositoryImpl : public RoomRepository, public SQLiteRepository {
    public:
        explicit RoomRepositoryImpl(const std::string& db_path) noexcept
            : SQLiteRepository(db_path) {
            create_table();
        }

        ::dungeons::Result<void> create_table() noexcept {
            std::string sql = R"(
                CREATE TABLE IF NOT EXISTS rooms (
                    uid_seed_id INTEGER NOT NULL,
                    uid_flags INTEGER NOT NULL,
                    uid_timestamp_year INTEGER NOT NULL,
                    uid_timestamp_month INTEGER NOT NULL,
                    uid_timestamp_day INTEGER NOT NULL,
                    uid_timestamp_millis INTEGER NOT NULL,
                    uid_timestamp_tz_offset INTEGER NOT NULL,
                    uid_random_id INTEGER NOT NULL,
                    room_number INTEGER NOT NULL,
                    location_uid_seed_id INTEGER,
                    location_uid_random_id INTEGER,
                    action_counter INTEGER NOT NULL,
                    previous_room_uid_seed_id INTEGER,
                    previous_room_uid_random_id INTEGER,
                    next_room_uid_seed_id INTEGER,
                    next_room_uid_random_id INTEGER,
                    PRIMARY KEY (uid_seed_id, uid_random_id)
                );
            )";
            return execute_sql(sql);
        }

        ::dungeons::Result<void> save(std::shared_ptr<Room> room) noexcept override {
            if (!room) return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT, "room is null");
            auto open_result = open_connection();
            if (!open_result) return open_result;
            std::string sql = "INSERT OR REPLACE INTO rooms VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
            sqlite3_stmt* stmt = nullptr;
            int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                if (stmt) sqlite3_finalize(stmt);
                return ::dungeons::Err(::dungeons::ErrorCode::IO_ERROR, "Не удалось подготовить запрос");
            }
            sqlite3_bind_int(stmt, 1, static_cast<int>(room->uid().seed_id()));
            sqlite3_bind_int(stmt, 2, static_cast<int>(room->uid().flags()));
            sqlite3_bind_int(stmt, 3, static_cast<int>(room->uid().timestamp().year()));
            sqlite3_bind_int(stmt, 4, static_cast<int>(room->uid().timestamp().month()));
            sqlite3_bind_int(stmt, 5, static_cast<int>(room->uid().timestamp().day()));
            sqlite3_bind_int(stmt, 6, static_cast<int>(room->uid().timestamp().milliseconds_from_midnight()));
            sqlite3_bind_int(stmt, 7, static_cast<int>(room->uid().timestamp().timezone_offset_minutes()));
            sqlite3_bind_int64(stmt, 8, static_cast<sqlite3_int64>(room->uid().random_id()));
            sqlite3_bind_int(stmt, 9, static_cast<int>(room->room_number()));
            auto loc = room->location().lock();
            if (loc) {
                sqlite3_bind_int(stmt, 10, static_cast<int>(loc->uid().seed_id()));
                sqlite3_bind_int64(stmt, 11, static_cast<sqlite3_int64>(loc->uid().random_id()));
            }
            else {
                sqlite3_bind_null(stmt, 10);
                sqlite3_bind_null(stmt, 11);
            }
            sqlite3_bind_int(stmt, 12, room->action_counter());
            auto prev = room->previous_room().lock();
            if (prev) {
                sqlite3_bind_int(stmt, 13, static_cast<int>(prev->uid().seed_id()));
                sqlite3_bind_int64(stmt, 14, static_cast<sqlite3_int64>(prev->uid().random_id()));
            }
            else {
                sqlite3_bind_null(stmt, 13);
                sqlite3_bind_null(stmt, 14);
            }
            auto next = room->next_room().lock();
            if (next) {
                sqlite3_bind_int(stmt, 15, static_cast<int>(next->uid().seed_id()));
                sqlite3_bind_int64(stmt, 16, static_cast<sqlite3_int64>(next->uid().random_id()));
            }
            else {
                sqlite3_bind_null(stmt, 15);
                sqlite3_bind_null(stmt, 16);
            }
            rc = sqlite3_step(stmt);
            sqlite3_finalize(stmt);
            if (rc != SQLITE_DONE)
                return ::dungeons::Err(::dungeons::ErrorCode::IO_ERROR, "Не удалось сохранить комнату");
            ::dungeons::Logger::instance().info("Room #" + std::to_string(room->room_number()) + " сохранена");
            return ::dungeons::Ok();
        }

        ::dungeons::Result<void> save_all(const std::vector<std::shared_ptr<Room>>& rooms) noexcept override {
            for (const auto& r : rooms) {
                auto rr = save(r);
                if (!rr) return rr;
            }
            return ::dungeons::Ok();
        }

        ::dungeons::Result<std::vector<std::shared_ptr<Room>>> load_all() noexcept override {
            auto open_result = open_connection();
            if (!open_result) return ::dungeons::Err<std::vector<std::shared_ptr<Room>>>(open_result.error().code(), open_result.error().message());
            std::string sql = "SELECT * FROM rooms;";
            sqlite3_stmt* stmt = nullptr;
            int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                if (stmt) sqlite3_finalize(stmt);
                return ::dungeons::Err<std::vector<std::shared_ptr<Room>>>(::dungeons::ErrorCode::IO_ERROR, "Не удалось подготовить запрос");
            }
            std::vector<std::shared_ptr<Room>> rooms;
            while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                uint32_t seed_id = static_cast<uint32_t>(sqlite3_column_int(stmt, 0));
                UidFlags flags = static_cast<UidFlags>(sqlite3_column_int(stmt, 1));
                ::dungeons::Time timestamp;
                timestamp.year(static_cast<uint16_t>(sqlite3_column_int(stmt, 2)));
                timestamp.month(static_cast<uint8_t>(sqlite3_column_int(stmt, 3)));
                timestamp.day(static_cast<uint8_t>(sqlite3_column_int(stmt, 4)));
                timestamp.milliseconds_from_midnight(static_cast<uint32_t>(sqlite3_column_int(stmt, 5)));
                timestamp.timezone_offset_minutes(static_cast<int16_t>(sqlite3_column_int(stmt, 6)));
                uint64_t random_id = static_cast<uint64_t>(sqlite3_column_int64(stmt, 7));
                auto uid = uid_t::from_repository(seed_id, flags, timestamp, random_id);
                size_t room_number = static_cast<size_t>(sqlite3_column_int(stmt, 8));
                int action_counter = sqlite3_column_int(stmt, 12);
                auto room = std::make_shared<Room>(uid, room_number, std::weak_ptr<Location>{}, action_counter);
                rooms.push_back(room);
            }
            sqlite3_finalize(stmt);
            ::dungeons::Logger::instance().info("Загружено Room: " + std::to_string(rooms.size()));
            return ::dungeons::Ok(rooms);
        }
    };

} // namespace dungeons::backend