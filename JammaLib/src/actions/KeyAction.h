#pragma once

#include "Action.h"

namespace actions
{
	class KeyAction :
		public base::Action
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
}
