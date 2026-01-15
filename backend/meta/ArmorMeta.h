// ArmorMeta.h
#pragma ocne


#include "ItemMeta.h"


namespace dungeons::backend{
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
}