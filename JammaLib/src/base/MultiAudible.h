#pragma once

#include <vector>
#include "../audio/AudioBuffer.h"

namespace base
{
	class MultiAudibleParams
	{
	public:
		unsigned int OutputBufferSize;
		unsigned int NumOutputChannels;
	};

	class MultiAudible
	{
	public:
		MultiAudible(MultiAudibleParams params);
		~MultiAudible();

	public:
		virtual void Play(std::vector<audio::AudioBuffer>& buf, unsigned int numSamps);

	protected:
		MultiAudibleParams _multiAudibleParams;
		std::vector<audio::AudioBuffer> _outputBuffers;
	};
}
