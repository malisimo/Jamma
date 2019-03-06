#include "AudioMixer.h"

AudioMixer::AudioMixer() :
	_channel(0)
{
}

AudioMixer::~AudioMixer()
{
}

void AudioMixer::AddAudio(std::vector<float> source,
	std::vector<float> scale,
	float* dest,
	unsigned int numSamps,
	unsigned int numChannels)
{

}