#pragma once


#include <cstdlib>
#include <ctime>


#define DNG_LEGACY_RND_GEN


// TODO: move macros to .cpp and write 2 implementations
// TODO: add modern implementation with <random>
using std::rand;


namespace dungeons {
	class Random {
#ifdef DNG_LEGACY_RND_GEN
		static thread_local unsigned int seed;

	public:
		static void init() {
			std::srand(static_cast<unsigned>(std::time(nullptr)));
		}

		static int randInt() {
			return rand() * rand() * rand() * (rand() % 20 + 1);
		}

		static double randDouble() {
			double res = static_cast<double>(randInt());
			res = res / INT_MAX;
			return res;
		}
#else
#endif
	};
}