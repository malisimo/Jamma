#pragma once

#include "../engine/Timer.h"

namespace base
{
	class Tickable
	{
	public:
		virtual void OnTick(Time curTime, unsigned int samps) = 0;
	};
}
