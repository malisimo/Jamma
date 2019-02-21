#pragma once

#include "Action.h"

class CursorAction :
	public Action
{
public:
	CursorAction();
	~CursorAction();

public:
	enum CursorActionType
	{
		CAPTURE_CHANGED,
		SET_CURSOR
	};

	unsigned int Cursor;
};

