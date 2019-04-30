#include "Loop.h"

using namespace base;
using namespace engine;
using namespace resources;
using base::AudioSink;
using audio::AudioMixer;
using audio::PanMixBehaviour;

Loop::Loop():
	ResourceUser(),
	_index(0),
	_loopParams({}),
	_wav(std::weak_ptr<WavResource>()),
	_mixer(std::make_unique<AudioMixer>())
{
	InitMixer();
}

Loop::Loop(LoopParams loopParams) :
	ResourceUser(),
	_index(0),
	_loopParams(loopParams),
	_wav(std::weak_ptr<WavResource>()),
	_mixer(std::make_unique<AudioMixer>())
{
	InitMixer();
}

bool Loop::InitResources(ResourceLib& resourceLib)
{
	auto resOpt = resourceLib.GetResource(_loopParams.Wav);

	if (!resOpt.has_value())
		return false;

	auto resource = resOpt.value().lock(); 
	
	if (!resource)
		return false;

	if (WAV != resource->GetType())
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

void Loop::Play(const std::vector<std::shared_ptr<AudioSink>>& dest, unsigned int numSamps)
{
	// Mixer will stereo spread the mono wav
	// and adjust level
	auto wav = _wav.lock();

	if (!wav)
		return;
	
	auto wavLength = wav->Length();
	auto wavBuf = wav->Buffer();
	auto index = _index;

	for (auto i = 0u; i < numSamps; i++)
	{
		_mixer->Play(dest, wavBuf[index], i);
		
		index++;
		if (index >= wavLength)
			index -= wavLength;
	}

	_mixer->Offset(dest, numSamps);

	_index += numSamps;
	_index %= wavLength;
}

void Loop::InitMixer()
{
	auto behaviour = std::make_unique<audio::PanMixBehaviour>();
	behaviour->ChannelLevels = { 0.8f, 0.2f };

	_mixer->SetBehaviour(std::move(behaviour));
}
