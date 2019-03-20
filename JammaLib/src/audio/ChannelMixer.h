#pragma once

#include "MultiAudible.h"

class ChannelMixerParams : public MultiAudibleParams
{
public:
	unsigned int InputBufferSize;
	unsigned int NumInputChannels;
};

class ChannelMixer : public MultiAudible
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
};

