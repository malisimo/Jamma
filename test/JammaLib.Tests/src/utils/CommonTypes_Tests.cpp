
#include "gtest/gtest.h"
#include "./utils/CommonTypes.h"

using utils::Position2d;

TEST(Position2d, CanSubtract) {
	Position2d posA = { 2, 13 };
	Position2d posB = { 8, 3 };
	auto res = posA - posB;

	ASSERT_EQ(-6, res.X);
	ASSERT_EQ(10, res.Y);
}