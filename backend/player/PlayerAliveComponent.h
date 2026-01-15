// PlayerAliveComponent.h 
#pragma once


#include "../backend_base.h"
#include "../Alive.h"


namespace dungeons::backend {
	class PlayerAliveComponent : public Alive {
	public:
		PlayerAliveComponent() : Alive() {}
		
		PlayerAliveComponent(std::string name) : Alive(name, 1, 100, 10, 10, Attributes(100, 10)) {}

		PlayerAliveComponent(const std::string& name, int32_t level, uint32_t base_health, int32_t base_defense, int32_t base_damage, const Attributes& base_attrs) 
			: Alive(name, level, base_health, base_defense, base_damage, base_attrs) {}

		friend class Player;
	};
}