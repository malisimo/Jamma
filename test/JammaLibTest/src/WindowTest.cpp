#include "pch.h"
#include "Window.h"

TEST(Window, IsWindowed) {
	auto win = Window();

	EXPECT_TRUE(win.GetConfig().Windowed);
}