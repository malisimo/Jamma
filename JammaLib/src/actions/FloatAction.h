#pragma once

#include "Action.h"

namespace actions
{
	class FloatAction :
		public base::Action
	{
	public:
		FloatAction();
		~FloatAction();

	public:
		float Value;
	};
}
