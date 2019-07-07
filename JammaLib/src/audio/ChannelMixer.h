#pragma once

#include "MultiAudioSource.h"
#include "MultiAudioSink.h"
#include "AudioBuffer.h"

namespace audio
{
	class ChannelMixerParams
	{
	public:
		unsigned int InputBufferSize;
		unsigned int OutputBufferSize;
		unsigned int NumInputChannels;
		unsigned int NumOutputChannels;
	};

	class ChannelMixer :
		public base::MultiAudioSource,
		public base::MultiAudioSink
	{
	public:
		ChannelMixer(ChannelMixerParams chanMixParams);
		~ChannelMixer();

	public:
		virtual MultiAudioDirection MultiAudibleDirection() const override
		{
			return MULTIAUDIO_BOTH;
		}
		virtual unsigned int NumInputChannels() const override;
		virtual unsigned int NumOutputChannels() const override;

		void SetParams(ChannelMixerParams chanMixParams);
		void FromAdc(float* inBuf, unsigned int numChannels, unsigned int numSamps);
		void ToDac(float* outBuf, unsigned int numChannels, unsigned int numSamps);

	protected:
		virtual const std::shared_ptr<base::AudioSink> InputChannel(unsigned int channel) override;
		virtual const std::shared_ptr<base::AudioSource> OutputChannel(unsigned int channel) override;

	public:
		static const unsigned int DefaultBufferSize = 3000;

	protected:
		std::vector<std::shared_ptr<AudioBuffer>> _inputBuffers;
		std::vector<std::shared_ptr<AudioBuffer>> _outputBuffers;
	};
}
