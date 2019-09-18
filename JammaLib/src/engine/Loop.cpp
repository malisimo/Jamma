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
using actions::ActionResult;
using actions::JobAction;

Loop::Loop(LoopParams loopParams,
	audio::AudioMixerParams mixerParams) :
	GuiElement(loopParams),
	AudioSink(),
	_modelNeedsUpdating(false),
	_playPos(0),
	_pitch(1.0),
	_length(0),
	_state(STATE_PLAYING),
	_loopParams(loopParams),
	_mixer(nullptr),
	_model(nullptr),
	_buffer({}),
	_backBuffer({})
{
	_mixer = std::make_unique<AudioMixer>(mixerParams);

	GuiModelParams modelParams;
	modelParams.Size = { 12, 14 };
	modelParams.ModelScale = .1f;
	modelParams.ModelTexture = "grid";
	modelParams.ModelShader = "texture_shaded";
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

audio::AudioMixerParams Loop::GetMixerParams(utils::Size2d loopSize,
	audio::BehaviourParams behaviour)
{
	AudioMixerParams mixerParams;
	mixerParams.Size = { 160, 320 };
	mixerParams.Position = { 6, 6 };
	mixerParams.Behaviour = behaviour;

	return mixerParams;
}

utils::Position2d Loop::Position() const
{
	auto pos = ModelPosition();
	return { (int)round(pos.X), (int)round(pos.Y) };
}

void Loop::SetSize(utils::Size2d size)
{
	auto mixerParams = GetMixerParams(size, audio::WireMixBehaviourParams());

	_mixer->SetSize(mixerParams.Size);

	GuiElement::SetSize(size);
}

void Loop::Draw3d(DrawContext& ctx)
{
	auto& glCtx = dynamic_cast<GlDrawContext&>(ctx);

	auto pos = ModelPosition();
	auto scale = ModelScale();

	auto frac = _length == 0 ? 0.0 : std::max(0.0, std::min(1.0, ((double)(_playPos % _length)) / ((double)_length)));

	_modelScreenPos = glCtx.ProjectScreen(pos);
	glCtx.PushMvp(glm::translate(glm::mat4(1.0), glm::vec3(pos.X, pos.Y, pos.Z)));
	glCtx.PushMvp(glm::scale(glm::mat4(1.0), glm::vec3(scale, scale, scale)));
	glCtx.PushMvp(glm::rotate(glm::mat4(1.0), (float)(TWOPI * frac), glm::vec3(0.0f, 1.0f, 0.0f)));

	for (auto& child : _children)
		child->Draw3d(ctx);

	glCtx.PopMvp();
	glCtx.PopMvp();
	glCtx.PopMvp();
}

void Loop::OnPlay(const std::shared_ptr<MultiAudioSink> dest,
	unsigned int numSamps)
{
	// Mixer will stereo spread the mono wav
	// and adjust level
	if (0 == _length)
		return;

	if (STATE_PLAYING != _state)
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
	if ((STATE_RECORDING != _state) &&
		(STATE_OVERDUBBING != _state) &&
		(STATE_PUNCHEDIN != _state))
		return indexOffset;

	auto bufSize = (unsigned long)_buffer.size();

	if (bufSize <= (_index + indexOffset))
	{
		if (bufSize >= _MaxBufferSize)
			return indexOffset;
		else
		{
			auto newBufSize = bufSize * 2;
			if (newBufSize > _MaxBufferSize)
				newBufSize = _MaxBufferSize;

			//_buffer.resize(newBufSize);
			// buffer must always be larger than _MaxFadeSamps!
			//_length = newBufSize - _MaxFadeSamps;
		}
	}
	else
		_buffer[_index + indexOffset] = samp;

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
	_length = _index;

	_modelNeedsUpdating = true;
	_changesMade = true;
}

void Loop::EndMultiPlay(unsigned int numSamps)
{
	if (STATE_PLAYING != _state)
		return;

	if (0 == _length)
		return;
		
	_playPos += numSamps;

	auto bufSize = _length + _MaxFadeSamps;
	while (_playPos > bufSize)
		_playPos -= _length;

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

	auto index = _playPos + delaySamps;
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

unsigned int Loop::InputChannel() const
{
	return _mixer->InputChannel();
}

void Loop::SetInputChannel(unsigned int channel)
{
	_mixer->SetInputChannel(channel);
}

unsigned long Loop::Id() const
{
	return _loopParams.Id;
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

	UpdateLoopModel();

	return true;
}

void Loop::Record()
{
	Reset();
	_state = STATE_RECORDING;
	_buffer = std::vector<float>(_MaxFadeSamps+90000);

	_changesMade = true;
}

void Loop::Play(unsigned long index, unsigned long length)
{
	auto bufSize = (unsigned int)_buffer.size();

	if (0 == bufSize)
	{
		Reset();
		return;
	}

	_playPos = (index + _MaxFadeSamps) >= bufSize ? (bufSize-1) : index + _MaxFadeSamps;
	_length = (length + _MaxFadeSamps) <= bufSize ? length : bufSize - _MaxFadeSamps;
	
	_state = length > 0 ? STATE_PLAYING : STATE_INACTIVE;
}

void Loop::Ditch()
{
	Reset();
	_buffer = std::vector<float>(_MaxFadeSamps); // TODO: backbuffer?
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

std::vector<JobAction> Loop::_CommitChanges()
{
	if (_modelNeedsUpdating)
	{
		_modelNeedsUpdating = false;

		JobAction job;
		job.JobActionType = JobAction::JOB_RENDERWAVE;
		job.Receiver = ActionReceiver::shared_from_this();
		return { job };
	}

	return {};
}

ActionResult Loop::OnAction(JobAction action)
{
	if (JobAction::JOB_RENDERWAVE == action.JobActionType)
	{
		UpdateLoopModel();
		ActionResult res;
		res.IsEaten = true;
		res.ResultType = actions::ACTIONRESULT_DEFAULT;

		return res;
	}

	return { false, actions::ACTIONRESULT_DEFAULT };
}

void Loop::Reset()
{
	_index = 0;
	_playPos = 0;
	_length = 0;
	_state = STATE_INACTIVE;
	_changesMade = true;
}

void Loop::UpdateLoopModel()
{
	std::vector<float> verts;
	std::vector<float> uvs;

	auto numSamps = (unsigned int)_buffer.size();
	auto lastYMin = -10.0f;
	auto lastYMax = 10.0f;
	auto numGrains = (unsigned int)ceil((double)numSamps / (double)_GrainSamps);
	auto radius = (float)CalcDrawRadius();

	for (auto grain = 1u; grain < numGrains; grain++)
	{
		auto [grainVerts, grainUvs,	nextYMin, nextYMax] =
			CalcGrainGeometry(grain,
			numGrains,
			lastYMin,
			lastYMax,
			radius);

		for (auto vert : grainVerts)
			verts.push_back(vert);

		for (auto uv : grainUvs)
			uvs.push_back(uv);

		lastYMin = nextYMin;
		lastYMax = nextYMax;
	}

	_model->SetGeometry(verts, uvs);
	_modelNeedsUpdating = false;
}

double Loop::CalcDrawRadius()
{
	auto minRadius = 200.0;
	auto maxRadius = 800.0;
	auto radius = 120.0 * log(_length) - 1000;

	return std::clamp(radius, 200.0, 800.0);
}

std::tuple<std::vector<float>, std::vector<float>, float, float>
	Loop::CalcGrainGeometry(unsigned int grain,
	unsigned int numGrains,
	float lastYMin,
	float lastYMax,
	float radius)
{
	const float minHeight = 4.0;
	const float radialThickness = radius / 20.0f;
	const float heightScale = 50.0f;

	auto yToUv = [minHeight, heightScale](float y) {
		auto scale = 1.0f / ((minHeight * 2) + (heightScale * 2));
		auto offset = 0.5f;
		return scale * y + offset;
	};

	auto angle1 = ((float)TWOPI) * ((float)(grain - 1) / (float)numGrains);
	auto angle2 = ((float)TWOPI) * ((float)grain / (float)numGrains);
	auto i1 = (grain - 1) * _GrainSamps;
	auto i2 = grain * _GrainSamps;
	auto gMin = utils::ArraySubMin(_buffer, i1, i2);
	auto gMax = utils::ArraySubMax(_buffer, i1, i2);

	auto xInner1 = sin(angle1) * (radius - radialThickness);
	auto xInner2 = sin(angle2) * (radius - radialThickness);
	auto xOuter1 = sin(angle1) * (radius + radialThickness);
	auto xOuter2 = sin(angle2) * (radius + radialThickness);
	auto yMin = (heightScale * gMin) - minHeight;
	auto yMax = (heightScale * gMax) + minHeight;
	auto zInner1 = cos(angle1) * (radius - radialThickness);
	auto zInner2 = cos(angle2) * (radius - radialThickness);
	auto zOuter1 = cos(angle1) * (radius + radialThickness);
	auto zOuter2 = cos(angle2) * (radius + radialThickness);

	std::vector<float> verts;
	std::vector<float> uvs;

	// Front
	verts.push_back(xOuter1); verts.push_back(lastYMin); verts.push_back(zOuter1);
	verts.push_back(xOuter2); verts.push_back(yMax); verts.push_back(zOuter2);
	verts.push_back(xOuter1); verts.push_back(lastYMax); verts.push_back(zOuter1);
	uvs.push_back(angle1 / (float)TWOPI); uvs.push_back(yToUv(lastYMin));
	uvs.push_back(angle2 / (float)TWOPI); uvs.push_back(yToUv(yMax));
	uvs.push_back(angle1 / (float)TWOPI); uvs.push_back(yToUv(lastYMax));

	verts.push_back(xOuter1); verts.push_back(lastYMin); verts.push_back(zOuter1);
	verts.push_back(xOuter2); verts.push_back(yMin); verts.push_back(zOuter2);
	verts.push_back(xOuter2); verts.push_back(yMax); verts.push_back(zOuter2);
	uvs.push_back(angle1 / (float)TWOPI); uvs.push_back(yToUv(lastYMin));
	uvs.push_back(angle2 / (float)TWOPI); uvs.push_back(yToUv(yMin));
	uvs.push_back(angle2 / (float)TWOPI); uvs.push_back(yToUv(yMax));

	// Top
	verts.push_back(xOuter1); verts.push_back(lastYMax); verts.push_back(zOuter1);
	verts.push_back(xInner2); verts.push_back(yMax); verts.push_back(zInner2);
	verts.push_back(xInner1); verts.push_back(lastYMax); verts.push_back(zInner1);
	uvs.push_back(angle1 / (float)TWOPI); uvs.push_back(yToUv(lastYMax));
	uvs.push_back(angle2 / (float)TWOPI); uvs.push_back(yToUv(yMax));
	uvs.push_back(angle1 / (float)TWOPI); uvs.push_back(yToUv(lastYMax));

	verts.push_back(xOuter1); verts.push_back(lastYMax); verts.push_back(zOuter1);
	verts.push_back(xOuter2); verts.push_back(yMax); verts.push_back(zOuter2);
	verts.push_back(xInner2); verts.push_back(yMax); verts.push_back(zInner2);
	uvs.push_back(angle1 / (float)TWOPI); uvs.push_back(yToUv(lastYMax));
	uvs.push_back(angle2 / (float)TWOPI); uvs.push_back(yToUv(yMax));
	uvs.push_back(angle2 / (float)TWOPI); uvs.push_back(yToUv(yMax));

	// Back
	verts.push_back(xInner1); verts.push_back(lastYMin); verts.push_back(zInner1);
	verts.push_back(xInner1); verts.push_back(lastYMax); verts.push_back(zInner1);
	verts.push_back(xInner2); verts.push_back(yMax); verts.push_back(zInner2);
	uvs.push_back(angle1 / (float)TWOPI); uvs.push_back(yToUv(lastYMin));
	uvs.push_back(angle1 / (float)TWOPI); uvs.push_back(yToUv(lastYMax));
	uvs.push_back(angle2 / (float)TWOPI); uvs.push_back(yToUv(yMax));

	verts.push_back(xInner1); verts.push_back(lastYMin); verts.push_back(zInner1);
	verts.push_back(xInner2); verts.push_back(yMax); verts.push_back(zInner2);
	verts.push_back(xInner2); verts.push_back(yMin); verts.push_back(zInner2);
	uvs.push_back(angle1 / (float)TWOPI); uvs.push_back(yToUv(lastYMin));
	uvs.push_back(angle2 / (float)TWOPI); uvs.push_back(yToUv(yMax));
	uvs.push_back(angle2 / (float)TWOPI); uvs.push_back(yToUv(yMin));

	// Bottom
	verts.push_back(xOuter1); verts.push_back(lastYMin); verts.push_back(zOuter1);
	verts.push_back(xInner1); verts.push_back(lastYMin); verts.push_back(zInner1);
	verts.push_back(xInner2); verts.push_back(yMin); verts.push_back(zInner2);
	uvs.push_back(angle1 / (float)TWOPI); uvs.push_back(yToUv(lastYMin));
	uvs.push_back(angle1 / (float)TWOPI); uvs.push_back(yToUv(lastYMin));
	uvs.push_back(angle2 / (float)TWOPI); uvs.push_back(yToUv(yMin));

	verts.push_back(xOuter1); verts.push_back(lastYMin); verts.push_back(zOuter1);
	verts.push_back(xInner2); verts.push_back(yMin); verts.push_back(zInner2);
	verts.push_back(xOuter2); verts.push_back(yMin); verts.push_back(zOuter2);
	uvs.push_back(angle1 / (float)TWOPI); uvs.push_back(yToUv(lastYMin));
	uvs.push_back(angle2 / (float)TWOPI); uvs.push_back(yToUv(yMin));
	uvs.push_back(angle2 / (float)TWOPI); uvs.push_back(yToUv(yMin));

	return std::make_tuple(verts, uvs, yMin, yMax);
}
