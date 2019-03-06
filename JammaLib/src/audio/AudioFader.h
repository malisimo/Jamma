#pragma once

#include <vector>

class AudioFader
{
public:
	AudioFader();
	~AudioFader();

public:
	void SetTarget(float target);
	virtual std::vector<float> OnAudio(unsigned int numSamps);

protected:
	float _target;
};

