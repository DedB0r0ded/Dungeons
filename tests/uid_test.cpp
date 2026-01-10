#include <gtest/gtest.h>
#include "../backend/uid.h"


TEST(UidTest, UidTypeSizeTest) {
	EXPECT_EQ(sizeof(::dungeons::uid_t), 32);
}