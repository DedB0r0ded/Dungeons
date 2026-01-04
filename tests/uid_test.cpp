#include <gtest/gtest.h>
#include "../uid.h"


TEST(UidTest, UidTypeSizeTest) {
	EXPECT_EQ(sizeof(::dungeons::uid_t), 32);
}