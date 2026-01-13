// ItemMeta.h
#pragma once


#include "./Entity.h"
#include "./Attributes.h"
#include <string>


namespace dungeons::backend {


    class ItemMeta : public Entity {
    protected:
        std::string name_;
        std::string description_;
        Attributes generation_basis_;


        // Protected constructor - cannot instantiate directly
        ItemMeta(const std::string& name, const std::string& description,
            const Attributes& basis, uint32_t seed_id)
            : Entity(seed_id, UidFlags::IS_ITEM),
            name_(name),
            description_(description),
            generation_basis_(basis) {
        }


    public:
        // Убираем конструктор по умолчанию
        ItemMeta() = delete;


        // Правило пяти
        ItemMeta(const ItemMeta&) = default;
        ItemMeta(ItemMeta&&) noexcept = default;
        ItemMeta& operator=(const ItemMeta&) = default;
        ItemMeta& operator=(ItemMeta&&) noexcept = default;
        virtual ~ItemMeta() = default;


        // Геттеры
        const std::string& name() const noexcept { return name_; }
        const std::string& description() const noexcept { return description_; }
        const Attributes& generation_basis() const noexcept { return generation_basis_; }


        // Сеттеры
        void name(const std::string& value) { name_ = value; }
        void description(const std::string& value) { description_ = value; }
        void generation_basis(const Attributes& value) { generation_basis_ = value; }
    };


    class ArmorMeta : public ItemMeta {
        float defense_basis_;


    public:
        ArmorMeta(const std::string& name, const std::string& description,
            const Attributes& basis, float defense_basis, uint32_t seed_id)
            : ItemMeta(name, description, basis, seed_id),
            defense_basis_(defense_basis) {
            uid_.set_flag(UidFlags::IS_ARMOR);
        }


        // Правило пяти
        ArmorMeta(const ArmorMeta&) = default;
        ArmorMeta(ArmorMeta&&) noexcept = default;
        ArmorMeta& operator=(const ArmorMeta&) = default;
        ArmorMeta& operator=(ArmorMeta&&) noexcept = default;
        ~ArmorMeta() = default;


        // Геттеры/Сеттеры
        float defense_basis() const noexcept { return defense_basis_; }
        void defense_basis(float value) noexcept { defense_basis_ = value; }
    };


    class WeaponMeta : public ItemMeta {
        float attack_basis_;


    public:
        WeaponMeta(const std::string& name, const std::string& description,
            const Attributes& basis, float attack_basis, uint32_t seed_id)
            : ItemMeta(name, description, basis, seed_id),
            attack_basis_(attack_basis) {
            uid_.set_flag(UidFlags::IS_WEAPON);
        }


        // Правило пяти
        WeaponMeta(const WeaponMeta&) = default;
        WeaponMeta(WeaponMeta&&) noexcept = default;
        WeaponMeta& operator=(const WeaponMeta&) = default;
        WeaponMeta& operator=(WeaponMeta&&) noexcept = default;
        ~WeaponMeta() = default;


        // Геттеры/Сеттеры
        float attack_basis() const noexcept { return attack_basis_; }
        void attack_basis(float value) noexcept { attack_basis_ = value; }
    };


} // namespace dungeons::backend