#pragma once

#include <vector>
#include "AudioSink.h"

namespace base
{
	class MultiAudioSource
	{
	public:
		virtual void Play(const std::vector<std::shared_ptr<base::AudioSink>>& dest, unsigned int numSamps) {};
	};
}
