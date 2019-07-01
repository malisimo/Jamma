#pragma once

#include <memory>
#include "../utils/CommonTypes.h"
#include "../engine/Timer.h"

namespace base
{
	class ActionReceiver;

	class Action
	{
	public:
		Action() {};
		~Action() {};

	public:
		Time GetActionTime() const { return _actionTime; }
		void SetActionTime(Time time) { _actionTime = time; }

	protected:
		Time _actionTime;
	};
}
