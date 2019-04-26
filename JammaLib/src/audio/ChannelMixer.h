#pragma once

#include "MultiAudioSource.h"
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

	class ChannelMixer : public base::MultiAudioSource
	{
	public:
		ChannelMixer(ChannelMixerParams chanMixParams);
		~ChannelMixer();

	public:
		void SetParams(ChannelMixerParams chanMixParams);
		void FromAdc(float* inBuf, unsigned int numChannels, unsigned int numSamps);
		void ToDac(float* outBuf, unsigned int numChannels, unsigned int numSamps);

	protected:
		std::vector<AudioBuffer> _inputBuffers;
		std::vector<AudioBuffer> _outputBuffers;
	};
}
