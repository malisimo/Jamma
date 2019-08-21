#include "Loop.h"

using namespace base;
using namespace engine;
using namespace resources;
using base::AudioSink;
using base::MultiAudioSource;
using audio::AudioMixer;
using audio::AudioMixerParams;
using audio::PanMixBehaviour;
using gui::GuiSliderParams;
using gui::GuiModel;
using gui::GuiModelParams;
using graphics::GlDrawContext;

Loop::Loop(LoopParams loopParams,
	audio::AudioMixerParams mixerParams) :
	GuiElement(loopParams),
	_playPos(0),
	_recPos(0),
	_pitch(1.0),
	_length(0),
	_state(STATE_PLAYING),
	_loopParams(loopParams),
	_mixer(nullptr),
	_model(nullptr)
{
	_mixer = std::make_unique<AudioMixer>(mixerParams);

	GuiModelParams modelParams;
	modelParams.Size = { 12, 14 };
	modelParams.ModelScale = 1.0f;
	modelParams.ModelTexture = "purple";
	modelParams.ModelShader = "texture";
	_model = std::make_shared<GuiModel>(modelParams);

	_children.push_back(_mixer);
	_children.push_back(_model);
}

std::optional<std::shared_ptr<Loop>> Loop::FromFile(LoopParams loopParams, io::JamFile::Loop loopStruct, std::wstring dir)
{
	audio::BehaviourParams behaviour;
	audio::WireMixBehaviourParams wire;
	audio::PanMixBehaviourParams pan;

	std::vector<unsigned long> chans;
	std::vector<double> levels;

	switch (loopStruct.Mix.Mix)
	{
	case io::JamFile::LoopMix::MIX_WIRE:
		if (loopStruct.Mix.Params.index() == 1)
			chans = std::get<std::vector<unsigned long>>(loopStruct.Mix.Params);

		for (auto chan : chans)
			wire.Channels.push_back(chan);

		behaviour = wire;
		break;
	case io::JamFile::LoopMix::MIX_PAN:
		if (loopStruct.Mix.Params.index() == 1)
			levels = std::get<std::vector<double>>(loopStruct.Mix.Params);

		for (auto level : levels)
			pan.ChannelLevels.push_back((float)level);

		behaviour = pan;
		break;
	}

	auto mixerParams = GetMixerParams(loopParams.Size, behaviour);

	loopParams.Wav = utils::EncodeUtf8(dir) + "/" + loopStruct.Name;
	auto loop = std::make_shared<Loop>(loopParams, mixerParams);

	loop->Load(io::WavReadWriter());
	loop->Play(loopStruct.MasterLoopCount, loopStruct.Length);

	return loop;
}

utils::Position2d Loop::Position() const
{
	auto pos = ModelPosition();
	return { pos.X, pos.Y };
}

void Loop::SetSize(utils::Size2d size)
{
	auto mixerParams = GetMixerParams(size, audio::WireMixBehaviourParams());

	_mixer->SetSize(mixerParams.Size);

	GuiElement::SetSize(size);
}

audio::AudioMixerParams Loop::GetMixerParams(utils::Size2d loopSize, audio::BehaviourParams behaviour)
{
	AudioMixerParams mixerParams;
	mixerParams.Size = { 160, 320 };
	mixerParams.Position = { 6, 6 };
	mixerParams.Behaviour = behaviour;

	return mixerParams;
}

void Loop::OnPlay(const std::shared_ptr<MultiAudioSink> dest,
	unsigned int numSamps)
{
	// Mixer will stereo spread the mono wav
	// and adjust level
	if (0 == _length)
		return;
	
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
	if (0 == _length)
		return;

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

bool Loop::Load(const io::WavReadWriter& readWriter)
{
	auto loadOpt = readWriter.Read(utils::DecodeUtf8(_loopParams.Wav), _MaxBufferSize);

	if (!loadOpt.has_value())
		return false;

	auto [buffer, sampleRate, bitDepth] = loadOpt.value();

	_length = 0;
	_buffer.clear();

	auto length = (unsigned long)buffer.size();
	_buffer = std::vector<float>(length);

	for (auto i = 0u; i < length; i++)
	{
		_buffer[i] = buffer[i];
	}

	_length = length - _MaxFadeSamps;

	return true;
}

void Loop::Record()
{
	_state = STATE_RECORDING;
}

void Loop::Play(unsigned long index, unsigned long length)
{
	auto bufSize = (unsigned int)_buffer.size();

	if (0 == bufSize)
	{
		_playPos = 0;
		_length = 0;
		_state = STATE_INACTIVE;
		return;
	}

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
