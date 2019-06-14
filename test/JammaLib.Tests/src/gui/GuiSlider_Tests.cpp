
#include "gtest/gtest.h"
#include "resources/ResourceLib.h"
#include "gui/GuiSlider.h"
#include "actions/TouchAction.h"
#include "actions/TouchMoveAction.h"

using resources::ResourceLib;
using gui::GuiSlider;
using gui::GuiSliderParams;
using actions::TouchAction;
using actions::TouchMoveAction;

TEST(GuiSlider, DoesUndo) {
	auto sliderParams = GuiSliderParams();
	sliderParams.DragLength = 100;
	sliderParams.Position = { 0, 0 };
	sliderParams.Size = { 20, 120 };
	sliderParams.DragControlSize = { 20, 20 };
	sliderParams.DragControlOffset = { 0, 10 };
	sliderParams.InitValue = 0.0;
	sliderParams.Orientation = GuiSliderParams::SLIDER_VERTICAL;

	auto slider = std::make_shared<GuiSlider>(sliderParams);
	auto v1 = slider->Value();

	ASSERT_EQ(0.0, slider->Value());

	auto downAction = TouchAction();
	downAction.Touch = actions::TouchAction::TOUCH_MOUSE;
	downAction.Position = { 10, 20 };
	downAction.Index = 0;
	downAction.State = TouchAction::TOUCH_DOWN;
	slider->OnAction(downAction);

	auto moveAction = TouchMoveAction();
	moveAction.Touch = actions::TouchAction::TOUCH_MOUSE;
	moveAction.Position = { 10, 120 };
	moveAction.Index = 0;
	slider->OnAction(moveAction);

	auto v2 = slider->Value();
	ASSERT_EQ(1.0, slider->Value());

	auto upAction = TouchAction();
	upAction.Touch = actions::TouchAction::TOUCH_MOUSE;
	upAction.Position = { 10, 120 };
	upAction.Index = 0;
	upAction.State = TouchAction::TOUCH_UP;
	auto res = slider->OnAction(upAction);

	ASSERT_EQ(true, res.IsEaten);
	
	res.Undo->Undo();

	auto v3 = slider->Value();
	ASSERT_EQ(0.0, slider->Value());
}