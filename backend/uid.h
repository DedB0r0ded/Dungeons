#include <cstdint>
#include "../time.h"

namespace dungeons::backend {
	enum class UidFlags : uint32_t {
		NONE = 0,
		IS_PLAYER = 1 << 0,
		IS_ENEMY = 1 << 1,
		IS_WEAPON = 1 << 2,
		IS_ARMOR = 1 << 3,
		IS_INVENTORY = 1 << 4,
	};
	
	class uid_t {
		uint32_t seed_id_;
		UidFlags flags_;
		::dungeons::Time timestamp_;
		uint64_t random_id_;

	public:
		uint32_t seed_id() const;
		UidFlags flags() const;
		::dungeons::Time timestamp() const;
		uint64_t random_id() const;
	};
}