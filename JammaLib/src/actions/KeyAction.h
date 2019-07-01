#pragma once

#include "Action.h"

namespace actions
{
	enum Modifier
	{
		MODIFIER_NONE = 0,
		MODIFIER_CTRL = 1,
		MODIFIER_ALT = 2,
		MODIFIER_SHIFT = 4
	};

	class KeyAction :
		public base::Action
	{
	public:
		KeyAction();
		~KeyAction();

	public:
		enum KeyActionType
		{
			KEY_UP,
			KEY_DOWN
		};

	public:
		Modifier Modifiers;
		KeyActionType KeyActionType;
		unsigned int KeyChar;
		bool IsSystem;
	};
}
