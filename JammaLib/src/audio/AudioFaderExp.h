#pragma once

#include "AudioFader.h"

class AudioFaderExp :
	public AudioFader
{
public:
	AudioFaderExp(float damping);
	~AudioFaderExp();

public:
	virtual std::vector<float> OnAudio(unsigned int numSamps) override;

protected:
	float _current;
	float _damping;
};

