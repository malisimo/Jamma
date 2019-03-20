#pragma once

#include <vector>
#include <memory>
#include "Audible.h"
#include "AudioBuffer.h"

class AudioMixer
{
public:
	AudioMixer();
	~AudioMixer();

public:
	void Tick(unsigned int numSamps);


protected:
	std::vector<std::shared_ptr<Audible>> _inputs;
	std::vector<std::shared_ptr<AudioBuffer>> _outputs;
};

