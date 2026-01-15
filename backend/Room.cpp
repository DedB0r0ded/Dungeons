#include "Room.h"


namespace dungeons::backend {


    // Геттер с валидацией
    dungeons::Result<void> Room::action_counter(int32_t value) noexcept {
        if (value < 0)
            return dungeons::Err(dungeons::ErrorCode::VALIDATION_FAILED, "Счетчик действий не может быть отрицательным");
        action_counter_ = value;
        return dungeons::Ok();
    }

    dungeons::Result<void> Room::validate_action_counter() const noexcept {
        if (action_counter_ <= 0) {
            dungeons::Logger::instance().warning("Попытка обыскать комнату без действий");
            return dungeons::Err(dungeons::ErrorCode::VALIDATION_FAILED, "Счетчик действий равен нулю");
        }
    }

    dungeons::Result<void> Room::validate_location(const std::shared_ptr<Location> p) const noexcept {
        if (!p) {
            dungeons::Logger::instance().error("Комната не связана с локацией");
            return dungeons::Err(dungeons::ErrorCode::INVALID_ARGUMENT, "Комната не связана с локацией");
        }
        return Ok();
    }


    Room::search_res_t Room::nothing_found() {
        ::dungeons::Logger::instance().info("Обыск комнаты: ничего не найдено");
        return ::dungeons::Ok(std::variant<std::monostate, Inventory, Enemy>(std::monostate{}));
    }

    Room::search_res_t Room::treasure_found(const Location& location_ptr) {
        auto inventory_factory = location_ptr.inventory_factory();
        if (!inventory_factory) {
            ::dungeons::Logger::instance().warning("InventoryFactory отсутствует, возвращаем пустой результат");
            return ::dungeons::Ok(std::variant<std::monostate, Inventory, Enemy>(std::monostate{}));
        }
        Inventory treasure = inventory_factory->create_treasure(room_number_);
        ::dungeons::Logger::instance().info(
            "Обыск комнаты: найдено сокровище с " +
            std::to_string(treasure.size()) + " предметами");
        return ::dungeons::Ok(std::variant<std::monostate, Inventory, Enemy>(treasure));
    }

    Room::search_res_t Room::enemy_found(const Location& location_ptr, const PlayerAliveComponent& player) {
        auto enemy_factory = location_ptr.enemy_factory();
        if (!enemy_factory) {
            ::dungeons::Logger::instance().warning("EnemyFactory отсутствует, возвращаем пустой результат");
            return ::dungeons::Ok(std::variant<std::monostate, Inventory, Enemy>(std::monostate{}));
        }
        auto enemy_result = enemy_factory->create_for_player(player);
        if (!enemy_result) {
            ::dungeons::Logger::instance().error("Ошибка создания врага: " + enemy_result.error().message());
            return ::dungeons::Err<std::variant<std::monostate, Inventory, Enemy>>(
                enemy_result.error().code(), enemy_result.error().message());
        }
        ::dungeons::Logger::instance().info("Обыск комнаты: найден противник '" + enemy_result.value().name() + "'");
        return ::dungeons::Ok(std::variant<std::monostate, Inventory, Enemy>(enemy_result.value()));
    }

    // Обыскать комнату (требует передачи Alive компонента игрока для генерации врагов)
    // Возвращает: monostate (ничего), Inventory (сокровище) или Enemy
    Room::search_res_t Room::search(const PlayerAliveComponent& player) noexcept {
        auto val_ac = validate_action_counter();
        if (!val_ac)
            return search_res_t(val_ac.error());
        auto location_ptr = location_.lock();
        auto val_loc = validate_location(location_ptr);
        if (!val_loc)
            return search_res_t(val_loc.error());
        action_counter_--;
        ::dungeons::Logger::instance().info(
            "Обыск комнаты #" + std::to_string(room_number_) +
            ", осталось действий: " + std::to_string(action_counter_));
        // Определение результата: 0 = ничего, 1 = сокровище, 2 = враг
        // Вероятности: 40% ничего, 30% сокровище, 30% враг
        float roll = ::dungeons::Random::next_float(0.0f, 1.0f);
        if (roll < 0.4f) {
            return nothing_found();
        }
        else if (roll < 0.7f) {
            return treasure_found(*location_ptr);
        }
        else {
            return enemy_found(*location_ptr, player);
        }
    }


    // Сбросить счетчик действий
    void Room::reset_actions() noexcept {
        action_counter_ = DEFAULT_ACTIONS;
        dungeons::Logger::instance().debug("Сброс счетчика действий комнаты #" + std::to_string(room_number_));
    }


    // Валидация
    dungeons::Result<void> Room::validate() const noexcept {
        if (!uid_.is_room())
            return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Экземпляр Room должен иметь IS_ROOM флаг");
        if (action_counter_ < 0)
            return ::dungeons::Err(::dungeons::ErrorCode::VALIDATION_FAILED, "Счетчик действий не может быть отрицательным");
        return ::dungeons::Ok();
    }


}