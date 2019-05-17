
#include "gtest/gtest.h"
#include "resources/ResourceLib.h"
#include "graphics/Window.h"
#include "engine/Scene.h"

using resources::ResourceLib;
using graphics::Window;
using engine::Scene;

TEST(Window, IsWindowed) {
	auto sceneParams = engine::SceneParams(base::DrawableParams(), base::SizeableParams(), base::AudioSourceParams());
	auto scene = Scene(sceneParams);

	ResourceLib resourceLib;
	auto win = Window(scene, resourceLib);

	ASSERT_EQ(graphics::Window::WindowState::WINDOWED, win.GetConfig().State);
}