#pragma once

#include <vector>
#include "Loop.h"

class LoopTake
{
public:
	LoopTake();
	~LoopTake();

protected:
	std::vector<Loop> _loops;
};

