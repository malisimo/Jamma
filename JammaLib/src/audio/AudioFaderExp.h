#pragma once

#include "Audible.h"
#include "AudioFader.h"

class AudioFaderExp :
	public AudioFader
{
public:
	AudioFaderExp(float damping);
	~AudioFaderExp();

public:
	virtual void Play(std::shared_ptr<AudioBuffer> buf, unsigned int numSamps) override;

protected:
	float _current;
	float _damping;
};

