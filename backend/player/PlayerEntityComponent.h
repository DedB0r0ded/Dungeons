// PlayerEntityComponent.h
#pragma once


#include "../backend_base.h"
#include "../Entity.h"


namespace dungeons::backend {
	class PlayerEntityComponent : public Entity {
	public:
		PlayerEntityComponent() : Entity() {
			uid_.set_flag(UidFlags::IS_PLAYER);
		}

		PlayerEntityComponent(uint32_t seed_id) : Entity(seed_id, UidFlags::IS_PLAYER) {}

		PlayerEntityComponent(uid_t uid) : Entity(uid) {}

		friend class Player;
	};
}