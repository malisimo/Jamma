#pragma once

#include "Action.h"
#include "CommonTypes.h"

class WindowAction :
	public Action
{
public:
	WindowAction();
	~WindowAction();

public:
	enum WindowEventType
	{
		SIZE,
		SIZE_MINIMISE,
		SIZE_MAXIMISE,
		ENTER_SIZE_MOVE,
		EXIT_SIZE_MOVE,
		DESTROY,
	};

	WindowEventType WindowEventType;
	Position2d Position;
	Size2d Size;
};

