// indexes.h
// TODO: fix newlines
// TODO: translate to russian (comments and error messages)
// TODO: remove redundant private modifiers
#pragma once


#include "./backend_base.h"
#include "./Entity.h"
#include "./meta/ArmorMeta.h"
#include "./meta/WeaponMeta.h"
#include "./meta/EnemyMeta.h"
#include "./Inventory.h"
#include "./Location.h"
#include "./Room.h"
#include "./player/Player.h"

#include <unordered_map>
#include <type_traits>
#include <algorithm>


namespace dungeons::backend {


    template<typename T>
    class BaseIndex {
    protected:
        std::unordered_map<uid_t, std::shared_ptr<T>> data_;

        BaseIndex() noexcept : data_() {}


    public:
        // Delete copy/move constructors and operators
        BaseIndex(const BaseIndex&) = delete;
        BaseIndex(BaseIndex&&) = delete;
        BaseIndex& operator=(const BaseIndex&) = delete;
        BaseIndex& operator=(BaseIndex&&) = delete;

        virtual ~BaseIndex() = default;



        // Add shared_ptr entity to index
        ::dungeons::Result<void> add(std::shared_ptr<T> entity) noexcept {
            if (!entity)
                return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT, "Cannot add null entity");
            data_[entity->uid()] = entity;
            return ::dungeons::Ok();
        }

        // Get entity by UID
        ::dungeons::Result<std::shared_ptr<T>> get(const uid_t& uid) noexcept {
            auto it = data_.find(uid);
            if (it == data_.end())
                return ::dungeons::Err<std::shared_ptr<T>>(::dungeons::ErrorCode::INVALID_ARGUMENT, "Entity not found in index");
            return ::dungeons::Ok(it->second);
        }

        // Remove entity by UID
        ::dungeons::Result<void> remove(const uid_t& uid) noexcept {
            auto it = data_.find(uid);
            if (it == data_.end())
                return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT, "Entity not found in index");
            data_.erase(it);
            return ::dungeons::Ok();
        }



        // Check if entity exists
        bool contains(const uid_t& uid) const noexcept {
            return data_.find(uid) != data_.end();
        }

        // Get size
        size_t size() const noexcept {
            return data_.size();
        }

        // Check if empty
        bool empty() const noexcept {
            return data_.empty();
        }

        // Clear all entities
        void clear() noexcept {
            data_.clear();
        }


        // Get all entities as vector of weak_ptr<const T>
        std::vector<std::weak_ptr<const T>> as_vector() const noexcept {
            std::vector<std::weak_ptr<const T>> result;
            result.reserve(data_.size());
            for (const auto& pair : data_) {
                result.push_back(std::weak_ptr<const T>(pair.second));
            }
            return result;
        }

        // Get all entities as vector of shared_ptr<T>
        std::vector<std::shared_ptr<T>> as_shared_vector() noexcept {
            std::vector<std::shared_ptr<T>> result;
            result.reserve(data_.size());
            for (auto& pair : data_) {
                result.push_back(pair.second);
            }
            return result;
        }


        // Select n random entities
        std::vector<std::weak_ptr<const T>> select_random(size_t n) const noexcept {
            if (n >= data_.size()) {
                return as_vector();
            }
            auto all = as_shared_vector_const();
            auto selected_shared = ::dungeons::Random::sample(all, n);
            std::vector<std::weak_ptr<const T>> result;
            result.reserve(selected_shared.size());
            for (const auto& ptr : selected_shared) {
                result.push_back(std::weak_ptr<const T>(ptr));
            }
            return result;
        }

        // Select n random entities (mutable version)
        std::vector<std::weak_ptr<T>> select_random_mutable(size_t n) noexcept {
            if (n >= data_.size()) {
                auto vec = as_shared_vector();
                std::vector<std::weak_ptr<T>> result;
                result.reserve(vec.size());
                for (auto& ptr : vec) {
                    result.push_back(std::weak_ptr<T>(ptr));
                }
                return result;
            }
            auto all = as_shared_vector();
            auto selected_shared = ::dungeons::Random::sample(all, n);
            std::vector<std::weak_ptr<T>> result;
            result.reserve(selected_shared.size());
            for (auto& ptr : selected_shared) {
                result.push_back(std::weak_ptr<T>(ptr));
            }
            return result;
        }      
    };

    // Specialized index classes

    class ArmorMetaIndex : public BaseIndex<ArmorMeta> {
    public:
        ArmorMetaIndex() noexcept : BaseIndex<ArmorMeta>() {}
        ~ArmorMetaIndex() = default;
    };

    class WeaponMetaIndex : public BaseIndex<WeaponMeta> {
    public:
        WeaponMetaIndex() noexcept : BaseIndex<WeaponMeta>() {}
        ~WeaponMetaIndex() = default;
    };

    class EnemyMetaIndex : public BaseIndex<EnemyMeta> {
    public:
        EnemyMetaIndex() noexcept : BaseIndex<EnemyMeta>() {}
        ~EnemyMetaIndex() = default;
    };

    class InventoryIndex : public BaseIndex<Inventory> {
    public:
        InventoryIndex() noexcept : BaseIndex<Inventory>() {}
        ~InventoryIndex() = default;
    };

    class LocationIndex : public BaseIndex<Location> {
    public:
        LocationIndex() noexcept : BaseIndex<Location>() {}
        ~LocationIndex() = default;
    };

    class RoomIndex : public BaseIndex<Room> {
    public:
        RoomIndex() noexcept : BaseIndex<Room>() {}
        ~RoomIndex() = default;
    };

    class PlayerIndex : public BaseIndex<Player> {
    public:
        PlayerIndex() noexcept : BaseIndex<Player>() {}
        ~PlayerIndex() = default;
    };

} // namespace dungeons::backend