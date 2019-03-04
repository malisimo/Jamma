#include "Loop.h"

Loop::Loop(LoopParams loopParams) :
	Audible(AudibleParams{}),
	ResourceUser(),
	_index(0),
	_loopParams(loopParams),
	_wav(std::weak_ptr<WavResource>())
{
}

Loop::~Loop()
{
}

bool Loop::InitResources(ResourceLib& resourceLib)
{
	auto resOpt = resourceLib.GetResource(_loopParams.Wav);

	if (!resOpt.has_value())
		return false;

	auto resource = resOpt.value().lock(); 
	
	if (!resource)
		return false;

	if (Resources::WAV != resource->GetType())
		return false;

	_wav = std::dynamic_pointer_cast<WavResource>(resource);

	return true;
}

bool Loop::ReleaseResources()
{
	auto wav = _wav.lock();

	if (wav)
		wav->Release();

	return true;
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
