#pragma once

#include <vector>
#include "../audio/AudioBuffer.h"

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
	virtual void Play(std::vector<AudioBuffer>& buf, unsigned int numSamps);

protected:
	MultiAudibleParams _multiAudibleParams;
	std::vector<AudioBuffer> _outputBuffers;
};

