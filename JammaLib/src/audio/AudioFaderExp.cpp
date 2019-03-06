#include "AudioFaderExp.h"

AudioFaderExp::AudioFaderExp(float damping) :
	_current(0.0f),
	_damping(damping)
{
}

AudioFaderExp::~AudioFaderExp()
{
}

std::vector<float> AudioFaderExp::OnAudio(unsigned int numSamps)
{
	auto newValue = _current;
	auto out = std::vector<float>(numSamps);

	for (unsigned int index = 0; index < numSamps; index++)
	{
		newValue = (newValue * _damping) + (1.0f - _damping) * _target;
		out[index] = newValue;
	}

	_current = newValue;

	return out;
}