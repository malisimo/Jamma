#pragma once

#include "MultiAudible.h"
#include "LoopTake.h"

namespace engine
{
	class Station : public base::MultiAudible
	{
	public:
		Station();
		~Station();

	protected:
		std::vector<LoopTake> _loopTakes;
	};
}
