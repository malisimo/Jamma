#pragma once

#include "Action.h"
#include "TouchAction.h"

class TouchMoveAction : public Action
{
public:
	TouchMoveAction();
	~TouchMoveAction();

public:
	TouchAction::TouchType Touch;
	int Index;
	Position2d Position;
};
