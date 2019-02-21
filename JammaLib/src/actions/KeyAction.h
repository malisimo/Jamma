#pragma once

#include "Action.h"

class KeyAction : public Action
{
public:
	KeyAction();
	~KeyAction();

public:
	enum KeyActionType
	{
		KEY_DOWN,
		KEY_UP
	};

public:
	KeyActionType KeyActionType;
	unsigned int KeyChar;
	bool IsSystem;
};

