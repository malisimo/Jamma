#include "AudioFader.h"

AudioFader::AudioFader() :
	_target(0.0f)
{
}

AudioFader::~AudioFader()
{
}

void AudioFader::SetTarget(float target)
{
	_target = target;
}

std::vector<float> AudioFader::OnAudio(unsigned int numSamps)
{
	auto out = std::vector<float>(numSamps);

	for (unsigned int index = 0; index < numSamps; index++)
	{
		out[index] = _target;
	}

	return out;
}