#pragma once

#include <vector>
#include "Loop.h"

namespace engine
{
	class LoopTake
	{
	public:
		LoopTake();
		~LoopTake();

	protected:
		std::vector<Loop> _loops;
	};
}
