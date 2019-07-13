#pragma once

#include <vector>
#include <memory>
#include "Audible.h"
#include "AudioSink.h"

namespace base
{
	class AudioSourceParams {};

	class AudioSource :
		public virtual Audible
	{
	public:
		AudioSource(AudioSourceParams params) :
			_sourceParams(params)
		{
		}

	public:
		virtual AudioDirection AudibleDirection() const override { return AUDIO_SOURCE; }
		virtual void OnPlay(const std::shared_ptr<base::AudioSink> dest,
			unsigned int numSamps) {};

		std::shared_ptr<AudioSource> shared_from_this()
		{
			return std::dynamic_pointer_cast<AudioSource>(
				Audible::shared_from_this());
		}

	protected:
		AudioSourceParams _sourceParams;
	};
}
