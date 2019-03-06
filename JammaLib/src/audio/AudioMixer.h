#pragma once

#include <vector>

class AudioMixer
{
public:
	AudioMixer();
	~AudioMixer();

public:
	void AddAudio(std::vector<float> source,
		std::vector<float> scale,
		float* dest,
		unsigned int numSamps,
		unsigned int numChannels);

protected:
	unsigned int _channel;
};

