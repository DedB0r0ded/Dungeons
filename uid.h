#include <cstdint>
#include "time.h"

namespace dungeons {
	class uid_t {
		uint32_t seed_id;
		uint32_t flags;
		::dungeons::Time timestamp;
		uint64_t random_entity_id;
	};
}