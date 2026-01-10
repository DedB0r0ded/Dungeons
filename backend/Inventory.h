#pragma once

#include "./backend_base.h"
#include "./Armor.h"
#include "./Weapon.h"
#include "./Money.h"


namespace dungeons::backend {


	class Inventory : public Entity {
	protected:
		Armor armor_;
		Weapon weapon_;
		Money money_;
		

	public:
		virtual const Armor& armor() = 0;
		virtual const Weapon& weapon() = 0;
		virtual const Money& money() = 0;
	};


}