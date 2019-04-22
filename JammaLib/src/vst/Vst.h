#pragma once

#include "Audible.h"

namespace vst
{
	class Vst :
		public base::Audible
	{
	public:
		Vst();
		~Vst();
	};
}
