#pragma once

#include <vector>
#include <memory>
#include "AudioSink.h"

namespace base
{
	class AudioSourceParams {};

	class AudioSource
	{
	public:
		AudioSource(AudioSourceParams params) :
			_sourceParams(params)
		{
		}

	public:
		virtual void Play(AudioSink& buf, unsigned int numSamps) {};

	protected:
		AudioSourceParams _sourceParams;
	};
}