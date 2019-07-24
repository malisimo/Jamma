#include "Loop.h"

using namespace base;
using namespace engine;
using namespace resources;
using base::AudioSink;
using base::MultiAudioSource;
using audio::AudioMixer;
using audio::AudioMixerParams;
using audio::PanMixBehaviour;

Loop::Loop(LoopParams loopParams) :
	GuiElement(loopParams),
	_playPos(0),
	_recPos(0),
	_pitch(1.0),
	_length(0),
	_state(STATE_PLAYING),
	_loopParams(loopParams),
	_wav(std::weak_ptr<WavResource>()),
	_mixer(nullptr)
{
	_mixer = std::make_unique<AudioMixer>(loopParams.MixerParams);
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

	_length = 0;
	_wav = std::dynamic_pointer_cast<WavResource>(resource);
	_buffer.clear();

	auto wav = _wav.lock();
	if (wav)
	{
		auto wavBuffer = wav->Buffer();
		auto length = (unsigned long)wavBuffer.size();
		_buffer = std::vector<float>(length);

		for (auto i = 0u; i < length; i++)
		{
			_buffer[i] = wavBuffer[i];
		}

		_length = length - _MaxFadeSamps;
	}

	return GuiElement::_InitResources(resourceLib);
}

bool Loop::_ReleaseResources()
{
	auto wav = _wav.lock();

	if (wav)
		wav->Release();

	return GuiElement::_ReleaseResources();
}

void Loop::OnPlay(const std::shared_ptr<MultiAudioSink> dest,
	unsigned int numSamps)
{
	// Mixer will stereo spread the mono wav
	// and adjust level
	auto wav = _wav.lock();

	if (!wav)
		return;

	if (0 == _length)
		return;
	
	//auto wavLength = wav->Length();
	//auto wavBuf = wav->Buffer();
	auto index = _index;
	auto bufSize = _length + _MaxFadeSamps;
	while (index >= bufSize)
		index -= _length;

	for (auto i = 0u; i < numSamps; i++)
	{
		_mixer->OnPlay(dest, _buffer[index], i);
		
		index++;
		if (index >= bufSize)
			index -= _length;
	}
}

int Loop::OnWrite(float samp, int indexOffset)
{
	return OnOverwrite(samp, indexOffset);
}

int Loop::OnOverwrite(float samp, int indexOffset)
{
	auto bufSize = (unsigned long)_buffer.size();

	if (bufSize < (_recPos + indexOffset))
	{
		if (bufSize >= _MaxBufferSize)
			return indexOffset;
		else
		{
			auto newBufSize = bufSize * 2;
			if (newBufSize > _MaxBufferSize)
				newBufSize = _MaxBufferSize;

			_buffer.resize(newBufSize);
			// buffer must always be larger than _MaxFadeSamps!
			_length = newBufSize - _MaxFadeSamps;
		}
	}

	_buffer[_recPos + indexOffset] = samp;

	return indexOffset + 1;
}

void Loop::EndWrite(unsigned int numSamps, bool updateIndex)
{
	// Only update if currently recording
	if ((STATE_RECORDING != _state) &&
		(STATE_OVERDUBBING != _state) &&
		(STATE_PUNCHEDIN != _state))
		return;

	if (!updateIndex)
		return;

	_index += numSamps;

	if (0 == _length)
		return;

	auto bufSize = _length + _MaxFadeSamps;
	while (_index > bufSize)
		_index -= _length;
}

void Loop::EndMultiPlay(unsigned int numSamps)
{
	// Only update if currently recording
	if (STATE_PLAYING != _state)
		return;

	if (0 == _length)
		return;
		
	_index += numSamps;

	auto bufSize = _length + _MaxFadeSamps;
	while (_index > bufSize)
		_index -= _length;

	for (unsigned int chan = 0; chan < NumOutputChannels(); chan++)
	{
		auto channel = OutputChannel(chan);
		channel->EndPlay(numSamps);
	}
}

void Loop::OnPlayRaw(const std::shared_ptr<base::MultiAudioSink> dest,
	unsigned int channel,
	unsigned int delaySamps,
	unsigned int numSamps)
{
	// Mixer will stereo spread the mono wav
	// and adjust level
	auto wav = _wav.lock();

	if (!wav)
		return;

	if (0 == _length)
		return;

	//auto wavLength = wav->Length();
	//auto wavBuf = wav->Buffer();
	auto index = _index + delaySamps;
	auto bufSize = _length + _MaxFadeSamps;
	while (index >= bufSize)
		index -= _length;

	for (auto i = 0u; i < numSamps; i++)
	{
		dest->OnWriteChannel(channel, _buffer[index], i);

		index++;
		if (index >= bufSize)
			index -= _length;
	}
}

unsigned int Loop::InputChannel()
{
	return _mixer->InputChannel();
}

void Loop::SetInputChannel(unsigned int channel)
{
	_mixer->SetInputChannel(channel);
}

void Loop::Record()
{
	_state = STATE_RECORDING;
}

void Loop::Play(unsigned long index, unsigned long length)
{
	auto bufSize = (unsigned int)_buffer.size();
	_playPos = (index + _MaxFadeSamps) >= bufSize ? (bufSize-1) : index + _MaxFadeSamps;
	_length = (length + _MaxFadeSamps) <= bufSize ? length : bufSize - _MaxFadeSamps;
	
	_state = length > 0 ? STATE_PLAYING : STATE_INACTIVE;
}

void Loop::Ditch()
{
	_recPos = 0;
	_playPos = 0;
	_length = 0;
	_state = STATE_INACTIVE;

	_buffer = std::vector<float>(_MaxFadeSamps);
}

void Loop::Overdub()
{
	_state = STATE_OVERDUBBING;
}

void Loop::PunchIn()
{
	_state = STATE_PUNCHEDIN;
}

void Loop::PunchOut()
{
	_state = STATE_OVERDUBBING;
}
