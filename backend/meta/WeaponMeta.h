// WeaponMeta.h
#pragma once


#include "ItemMeta.h"


namespace dungeons::backend {
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