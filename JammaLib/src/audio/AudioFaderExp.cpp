#include "AudioFaderExp.h"

AudioFaderExp::AudioFaderExp(float damping) :
	_current(0.0f),
	_damping(damping)
{
}

AudioFaderExp::~AudioFaderExp()
{
}

void AudioFaderExp::Play(std::shared_ptr<AudioBuffer> buf, unsigned int numSamps)
{
	AudioFader::Play(buf, numSamps);

	auto newValue = _current;
	auto out = std::vector<float>(numSamps);

	for (unsigned int index = 0; index < numSamps; index++)
	{
		newValue = (newValue * _damping) + (1.0f - _damping) * _target;
		buf->Push(newValue);
	}

	_current = newValue;
}