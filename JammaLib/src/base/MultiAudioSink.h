#pragma once

#include <vector>
#include <optional>
#include "AudioSink.h"
#include "MultiAudible.h"

namespace base
{
	class MultiAudioSource;

	class MultiAudioSink :
		public virtual MultiAudible
	{
	public:
		virtual MultiAudioDirection MultiAudibleDirection() const override
		{
			return MULTIAUDIO_SINK;
		}

		virtual void Zero(unsigned int numSamps)
		{
			for (auto chan = 0u; chan < NumInputChannels(); chan++)
			{
				auto channel = InputChannel(chan);
				channel->Zero(numSamps);
			}
		}
		virtual void OnWrite(const std::shared_ptr<base::MultiAudioSource> src,
			unsigned int numSamps) {}
		virtual void EndMultiWrite(unsigned int numSamps) { return EndMultiWrite(numSamps, false); }
		virtual void EndMultiWrite(unsigned int numSamps, bool updateIndex)
		{
			for (auto chan = 0u; chan < NumInputChannels(); chan++)
			{
				auto channel = InputChannel(chan);
				channel->EndWrite(numSamps, updateIndex);
			}
		}
		virtual void OnWriteChannel(unsigned int channel,
			const std::shared_ptr<base::AudioSource> src,
			unsigned int numSamps)
		{
			auto chan = InputChannel(channel);
			if (chan)
				src->OnPlay(chan, numSamps);
		}
		virtual void OnWriteChannel(unsigned int channel,
			float samp,
			unsigned int indexOffset)
		{
			auto chan = InputChannel(channel);
			if (chan)
				chan->OnWrite(samp, indexOffset);
		}
		virtual unsigned int NumInputChannels() const { return 0; };

		std::shared_ptr<MultiAudioSink> shared_from_this()
		{
			return std::dynamic_pointer_cast<MultiAudioSink>(
				MultiAudible::shared_from_this());
		}

	protected:
		virtual const std::shared_ptr<AudioSink> InputChannel(unsigned int channel) { return std::shared_ptr<AudioSink>(); }
	};
}
