#pragma once

#include "MultiAudible.h"
#include "LoopTake.h"

class Station :	public MultiAudible
{
public:
	Station();
	~Station();

protected:
	std::vector<LoopTake> _loopTakes;
};

