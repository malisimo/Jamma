#pragma once

#include "Action.h"
#include "CommonTypes.h"

class TouchAction : public Action
{
public:
	TouchAction();
	~TouchAction();

public:
	enum TouchState
	{
		TOUCH_DOWN,
		TOUCH_UP
	};

	enum TouchType
	{
		TOUCH_MOUSE,
		TOUCH_FINGER
	};

public:
	TouchState State;
	TouchType Touch;
	int Index;
	int Value;
	Position2d Position;
};

