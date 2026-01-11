#pragma once


#include "backend_base.h"
#include "Entity.h"
#include "Enemy.h"


namespace dungeons::backend {


	class Location : public Entity {
		std::string name_;
		std::string description_;
		std::vector<Enemy> enemy_pull_;

	};


}