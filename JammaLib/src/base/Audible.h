#pragma once

class AudibleParams
{
};

class Audible
{
public:
	Audible(AudibleParams params);
	~Audible();

public:
	virtual void Play(float* buf, unsigned int numChans, unsigned int numSamps);

protected:
	AudibleParams _audibleParams;
};

