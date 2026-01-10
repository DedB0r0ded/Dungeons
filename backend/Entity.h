#pragma once


#include "./backend_base.h"


namespace dungeons::backend {


	class Entity {
	protected:
		uid_t uid_;

	public:
		uid_t uid() const;
	};



}