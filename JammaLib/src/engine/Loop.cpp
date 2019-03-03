#include "Loop.h"

Loop::Loop() :
	Audible(AudibleParams{}),
	_index(0),
	_wav(std::weak_ptr<WavResource>())
{
}

Loop::~Loop()
{
}

void Loop::Play(float* buf, unsigned int numChans, unsigned int numSamps)
{
	if (nullptr == buf)
		return;

	auto wav = _wav.lock();

	if (!wav)
		return;

	auto wavBuf = wav->Buffer();
	auto index = _index;

	for (unsigned int i = 0; i < numSamps; i++)
	{
		buf[i*numChans] += wavBuf[index++];
	}

	_index += numSamps;
	_index %= wav->Length();
}
