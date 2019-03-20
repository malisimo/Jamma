#pragma once

#include <vector>
#include <memory>

class AudioBuffer;
class AudibleParams
{
};

class Audible
{
public:
	Audible(AudibleParams params);
	~Audible();

public:
	virtual void Play(std::shared_ptr<AudioBuffer> buf, unsigned int numSamps);

protected:
	AudibleParams _audibleParams;
};

