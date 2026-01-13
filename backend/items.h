// items.h
#pragma once


#include "./ItemMeta.h"
#include "./Money.h"
#include "./Attributes.h"
#include "../result.h"

#include <type_traits>
#include <memory>
#include <algorithm>


namespace dungeons::backend {
    

    class IItem {
    public:
        virtual std::weak_ptr<ItemMeta> meta() const noexcept = 0;

        //Доступ к uid напрямую
        virtual const Result<uid_t> uid() const noexcept {
            auto mt = meta().lock();
            if (!mt)
                return Err<uid_t>(ErrorCode::VALIDATION_FAILED, "Метаданные предмета не инициализированы.");
            return Result(mt->uid());
        }
    };


    class Item : public IItem {

    protected:
        const Money value_;
        Attributes attributes_;
        int32_t current_durability_;
        int32_t max_durability_;
        bool breakable_;
        std::weak_ptr<ItemMeta> meta_;

        // Protected конструктор - нельзя инстанцировать напрямую
        Item(const Money& value, const Attributes& attrs,
            int32_t max_dur, bool breakable, std::weak_ptr<ItemMeta> meta)
            : value_(value),
            attributes_(attrs),
            current_durability_(max_dur),
            max_durability_(max_dur),
            breakable_(breakable),
            meta_(meta) {
        }


    public:
        // Убираем конструктор по умолчанию
        Item() = delete;


        // Правило пяти
        Item(const Item&) = default;
        Item(Item&&) noexcept = default;
        Item& operator=(const Item&) = default;
        Item& operator=(Item&&) noexcept = default;
        virtual ~Item() = default;


        // Геттеры
        const Money& value() const noexcept { return value_; }
        const Attributes& attributes() const noexcept { return attributes_; }
        int32_t current_durability() const noexcept { return current_durability_; }
        int32_t max_durability() const noexcept { return max_durability_; }
        bool breakable() const noexcept { return breakable_; }
        std::weak_ptr<ItemMeta> meta() const noexcept override { return meta_; }


        // Сеттеры с валидацией
        void attributes(const Attributes& value) noexcept {
            attributes_ = value;
        }

        ::dungeons::Result<void> current_durability(int32_t value) noexcept {
            if (value < 0)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Прочность не может быть отрицательной");
            if (value > max_durability_)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Текущая прочность не может превышать максимум");
            current_durability_ = value;
            return ::dungeons::Ok();
        }

        ::dungeons::Result<void> max_durability(int32_t value) noexcept {
            if (value < 0)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Максимальная прочность не может быть отрицательной");
            max_durability_ = value;
            if (current_durability_ > max_durability_) {
                current_durability_ = max_durability_;
            }
            return ::dungeons::Ok();
        }


        // Валидация
        ::dungeons::Result<void> validate() const noexcept {
            if (current_durability_ < 0)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Текущая прочность меньше нуля");
            if (max_durability_ < 0) 
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Максимальная прочность меньше нуля");
            if (current_durability_ > max_durability_)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Текущая прочность превышает максимум");
            auto value_validation = value_.validate();
            if (!value_validation)
                return value_validation;
            return ::dungeons::Ok();
        }


        bool is_broken() const noexcept {
            return breakable_ && current_durability_ <= 0;
        }

        // Уменьшить прочность предмета
        ::dungeons::Result<void> damage(int32_t amount) noexcept {
            if (!breakable_)
                return ::dungeons::Ok();
            if (amount < 0)
                return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT, "Урон не может быть отрицательным");
            current_durability_ = (current_durability_ > amount) ? (current_durability_ - amount) : 0;
            return ::dungeons::Ok();
        }

        // Починить предмет
        ::dungeons::Result<void> repair(int32_t amount) noexcept {
            if (amount < 0)
                return ::dungeons::Err(::dungeons::ErrorCode::INVALID_ARGUMENT, "Значение починки не может быть отрицательным");
            current_durability_ = std::min(current_durability_ + amount, max_durability_);
            return ::dungeons::Ok();
        }

        // Полностью починить предмет
        ::dungeons::Result<void> repair_fully() noexcept {
            current_durability_ = max_durability_;
            return ::dungeons::Ok();
        }
    };


    class Armor : public Item {
        int32_t defense_;


    public:
        Armor(const Money& value, const Attributes& attrs, int32_t max_dur,
            bool breakable, std::weak_ptr<ArmorMeta> meta, int32_t defense)
            : Item(value, attrs, max_dur, breakable, meta),
            defense_(defense) {
        }


        // Правило пяти
        Armor(const Armor&) = default;
        Armor(Armor&&) noexcept = default;
        Armor& operator=(const Armor&) = default;
        Armor& operator=(Armor&&) noexcept = default;
        ~Armor() = default;


        // Геттеры/сеттеры
        int32_t defense() const noexcept { return defense_; }

        ::dungeons::Result<void> defense(int32_t value) noexcept {
            if (value < 0)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Защита не может быть отрицательной");
            defense_ = value;
            return ::dungeons::Ok();
        }


        // Валидация
        ::dungeons::Result<void> validate() const noexcept {
            auto base_validation = Item::validate();
            if (!base_validation)
                return base_validation;
            if (defense_ < 0)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Защита меньше нуля");
            return ::dungeons::Ok();
        }
    };


    class Weapon : public Item {
        int32_t base_attack_;


    public:
        Weapon(const Money& value, const Attributes& attrs, int32_t max_dur,
            bool breakable, std::weak_ptr<WeaponMeta> meta, int32_t attack)
            : Item(value, attrs, max_dur, breakable, meta),
            base_attack_(attack) {
        }


        // Правило пяти
        Weapon(const Weapon&) = default;
        Weapon(Weapon&&) noexcept = default;
        Weapon& operator=(const Weapon&) = default;
        Weapon& operator=(Weapon&&) noexcept = default;
        ~Weapon() = default;


        // Геттеры/Сеттеры
        int32_t base_attack() const noexcept { return base_attack_; }

        ::dungeons::Result<void> base_attack(int32_t value) noexcept {
            if (value < 0)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Атака не может быть отрицательной");
            base_attack_ = value;
            return ::dungeons::Ok();
        }


        // Валидация
        ::dungeons::Result<void> validate() const noexcept {
            auto base_validation = Item::validate();
            if (!base_validation)
                return base_validation;
            if (base_attack_ < 0)
                return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Базовая атака меньше нуля");
            return ::dungeons::Ok();
        }
    };


} // namespace dungeons::backend