#pragma once

#include <vector>
#include "AudioSource.h"
#include "AudioSink.h"
#include "MultiAudible.h"
#include "MultiAudioSink.h"

namespace base
{
	class MultiAudioSource :
		public virtual MultiAudible
	{
	public:
		MultiAudioSource() {}
		~MultiAudioSource() {}

	public:
		virtual MultiAudioDirection MultiAudibleDirection() const override { return MULTIAUDIO_SOURCE; }
		virtual void OnPlay(const std::shared_ptr<base::MultiAudioSink> dest,
			unsigned int numSamps)
		{
			for (unsigned int chan = 0; chan < NumOutputChannels(); chan++)
			{
				auto channel = OutputChannel(chan);
				if (channel)
					dest->OnWriteChannel(chan, channel, numSamps);
			}
		}
		virtual void OnPlayChannel(unsigned int channel,
			const std::shared_ptr<base::AudioSink> dest,
			unsigned int numSamps)
		{
			auto chan = OutputChannel(channel);
			chan->OnPlay(dest, numSamps);
		}
		virtual unsigned int NumOutputChannels() const { return 0; };

		std::shared_ptr<MultiAudioSource> shared_from_this()
		{
			return std::dynamic_pointer_cast<MultiAudioSource>(
				Sharable::shared_from_this());
		}

	protected:
		virtual const std::shared_ptr<AudioSource> OutputChannel(unsigned int channel) { return std::shared_ptr<AudioSource>(); }
	};
}
