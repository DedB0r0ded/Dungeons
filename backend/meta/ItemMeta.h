// ItemMeta.h
#pragma once


#include "../Entity.h"
#include "../Attributes.h"


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
            name_(name), description_(description), generation_basis_(basis) {
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
}