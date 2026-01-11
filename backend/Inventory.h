// Inventory.h
// TODO: change all error messages to russian
#pragma once


#include "./Entity.h"
#include "./Item.h"
#include "./Money.h"
#include "./backend_base.h"

#include <vector>
#include <memory>
#include <optional>
#include <algorithm>


namespace dungeons::backend {


    class BaseInventory {
    protected:
        std::vector<std::optional<std::shared_ptr<Item<ItemMeta>>>> items_;
        size_t max_size_;
        Money money_;


    public:
        // Конструкторы
        BaseInventory() noexcept : max_size_(20), money_() {
            items_.resize(max_size_);
        }

        explicit BaseInventory(size_t max_size) noexcept
            : max_size_(max_size), money_() {
            items_.resize(max_size_);
        }

        BaseInventory(size_t max_size, const Money& money) noexcept
            : max_size_(max_size), money_(money) {
            items_.resize(max_size_);
        }


        // Правило пяти
        BaseInventory(const BaseInventory&) = default;
        BaseInventory(BaseInventory&&) noexcept = default;
        BaseInventory& operator=(const BaseInventory&) = default;
        BaseInventory& operator=(BaseInventory&&) noexcept = default;
        virtual ~BaseInventory() = default;


        // Геттеры
        size_t max_size() const noexcept { return max_size_; }
        const Money& money() const noexcept { return money_; }
        Money& money() noexcept { return money_; }

        size_t size() const noexcept {
            return std::count_if(items_.begin(), items_.end(),
                [](const auto& item) { return item.has_value(); });
        }

        size_t free_slots() const noexcept {
            return max_size_ - size();
        }

        bool is_empty() const noexcept {
            return size() == 0;
        }

        bool is_full() const noexcept {
            return size() == max_size_;
        }


        // Получить предмет по id
        ::dungeons::Result<std::shared_ptr<Item<ItemMeta>>> get_item(size_t id) const noexcept {
            if (id >= max_size_) {
                return ::dungeons::Err<std::shared_ptr<Item<ItemMeta>>>(
                    ::dungeons::ErrorCode::OUT_OF_RANGE,
                    "Item ID out of range");
            }
            if (!items_[id].has_value()) {
                return ::dungeons::Err<std::shared_ptr<Item<ItemMeta>>>(
                    ::dungeons::ErrorCode::INVALID_ARGUMENT,
                    "Slot is empty");
            }
            return ::dungeons::Ok(items_[id].value());
        }


        bool is_slot_empty(size_t id) const noexcept {
            if (id >= max_size_) return true;
            return !items_[id].has_value();
        }


        // Положить предмет в первый свободный слот
        ::dungeons::Result<size_t> add_item(std::shared_ptr<Item<ItemMeta>> item) noexcept {
            if (!item)
                return ::dungeons::Err<size_t>(::dungeons::ErrorCode::INVALID_ARGUMENT, "Cannot add null item");
            for (size_t i = 0; i < max_size_; ++i) {
                if (!items_[i].has_value()) {
                    items_[i] = item;
                    return ::dungeons::Ok(i);
                }
            }
            return ::dungeons::Err<size_t>(::dungeons::ErrorCode::VALIDATION_FAILED, "Inventory is full");
        }

        // Положить предмет в конкретный слот
        ::dungeons::Result<void> add_item_at(std::shared_ptr<Item<ItemMeta>> item, size_t id) noexcept {
            if (!item)
                return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT, "Cannot add null item");
            if (id >= max_size_)
                return ::dungeons::Err(::dungeons::ErrorCode::OUT_OF_RANGE, "Item ID out of range");
            if (items_[id].has_value())
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Slot is already occupied");
            items_[id] = item;
            return ::dungeons::Ok();
        }

        // Убрать предмет по id
        ::dungeons::Result<void> remove_item(size_t id) noexcept {
            if (id >= max_size_)
                return ::dungeons::Err(::dungeons::ErrorCode::OUT_OF_RANGE, "Item ID out of range");
            if (!items_[id].has_value())
                return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT, "Slot is already empty");
            items_[id].reset();
            return ::dungeons::Ok();
        }

        // Переместить предмет внутри инвентаря
        ::dungeons::Result<void> move_item(size_t from_id, size_t to_id) noexcept {
            if (from_id >= max_size_ || to_id >= max_size_)
                return ::dungeons::Err(::dungeons::ErrorCode::OUT_OF_RANGE, "Item ID out of range");
            if (!items_[from_id].has_value())
                return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT, "Source slot is empty");
            // Поменять местами
            std::swap(items_[from_id], items_[to_id]);
            return ::dungeons::Ok();
        }

        // Переместить предмет из одного инвентаря в другой
        ::dungeons::Result<void> transfer_item(size_t source_id, size_t target_id,
            BaseInventory& target) noexcept {
            if (source_id >= max_size_)
                return ::dungeons::Err(::dungeons::ErrorCode::OUT_OF_RANGE, "Source ID out of range");
            if (target_id >= target.max_size_)
                return ::dungeons::Err(::dungeons::ErrorCode::OUT_OF_RANGE, "Target ID out of range");
            if (!items_[source_id].has_value())
                return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT, "Source slot is empty");
            // Если оба слота заняты - меняем местами
            if (target.items_[target_id].has_value()) {
                std::swap(items_[source_id], target.items_[target_id]);
            }
            // Просто перемещаем
            else {
                target.items_[target_id] = items_[source_id];
                items_[source_id].reset();
            }
            return ::dungeons::Ok();
        }

        // Переместить все предметы в другой инвентарь
        ::dungeons::Result<void> transfer_all(BaseInventory& target) noexcept {
            size_t transferred = 0;
            size_t target_free = target.free_slots();
            for (size_t i = 0; i < max_size_ && transferred < target_free; ++i) {
                if (items_[i].has_value()) {
                    auto result = target.add_item(items_[i].value());
                    if (result) {
                        items_[i].reset();
                        transferred++;
                    }
                }
            }
            auto money_result = target.money_.add(money_);
            if (money_result)
                money_ = Money(0);
            return ::dungeons::Ok();
        }


        std::vector<std::shared_ptr<Item<ItemMeta>>> get_all_items() const noexcept {
            std::vector<std::shared_ptr<Item<ItemMeta>>> result;
            for (const auto& item_opt : items_) {
                if (item_opt.has_value()) {
                    result.push_back(item_opt.value());
                }
            }
            return result;
        }


        void clear() noexcept {
            for (auto& item : items_) {
                item.reset();
            }
            money_ = Money(0);
        }


        // Валидация
        ::dungeons::Result<void> validate() const noexcept {
            if (max_size_ == 0)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Max size cannot be zero");
            auto money_validation = money_.validate();
            if (!money_validation)
                return money_validation;
            return ::dungeons::Ok();
        }
    };


    class Inventory : public BaseInventory, public Entity {
    public:
        // Конструкторы
        Inventory() noexcept : BaseInventory(), Entity() {
            uid_.set_flag(UidFlags::IS_INVENTORY);
        }

        explicit Inventory(size_t max_size) noexcept
            : BaseInventory(max_size), Entity() {
            uid_.set_flag(UidFlags::IS_INVENTORY);
        }

        Inventory(size_t max_size, const Money& money) noexcept
            : BaseInventory(max_size, money), Entity() {
            uid_.set_flag(UidFlags::IS_INVENTORY);
        }

        Inventory(const uid_t& uid, size_t max_size, const Money& money) noexcept
            : BaseInventory(max_size, money), Entity(uid) {
        }


        // Правило пяти
        Inventory(const Inventory&) = default;
        Inventory(Inventory&&) noexcept = default;
        Inventory& operator=(const Inventory&) = default;
        Inventory& operator=(Inventory&&) noexcept = default;
        ~Inventory() = default;


        // Валидация
        ::dungeons::Result<void> validate() const noexcept {
            auto base_validation = BaseInventory::validate();
            if (!base_validation)
                return base_validation;
            if (!uid_.is_inventory())
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Inventory must have IS_INVENTORY flag");
            return ::dungeons::Ok();
        }
    };


} // namespace dungeons::backend