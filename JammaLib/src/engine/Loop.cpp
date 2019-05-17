#include "Loop.h"

using namespace base;
using namespace engine;
using namespace resources;
using base::AudioSink;
using audio::AudioMixer;
using audio::AudioMixerParams;
using audio::PanMixBehaviour;

Loop::Loop(LoopParams loopParams) :
	GuiElement(loopParams),
	_index(0),
	_loopParams(loopParams),
	_wav(std::weak_ptr<WavResource>()),
	_mixer(std::make_unique<AudioMixer>(loopParams.MixerParams))
{
	_children.push_back(_mixer);
}

bool Loop::_InitResources(ResourceLib& resourceLib)
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

	return GuiElement::InitResources(resourceLib);
}

bool Loop::_ReleaseResources()
{
	auto wav = _wav.lock();

	if (wav)
		wav->Release();

	return GuiElement::ReleaseResources();
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
