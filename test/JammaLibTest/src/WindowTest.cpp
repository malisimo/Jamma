#include "pch.h"
#include "Window.h"

TEST(Window, IsWindowed) {
	auto scene = Scene();
	auto win = Window(scene);

	EXPECT_TRUE(win.GetConfig().Windowed);
}