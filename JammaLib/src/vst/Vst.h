#pragma once

#include "AudioSource.h"

namespace vst
{
	class Vst :
		public base::AudioSource
	{
	public:
		Vst();
		~Vst();
	};
}
