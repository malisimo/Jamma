#include "LoopTake.h"

using namespace engine;
using base::AudioSink;
using base::AudioSource;
using base::DrawContext;
using base::Drawable;
using base::DrawableParams;
using engine::Trigger;
using resources::ResourceLib;
using actions::ActionResult;
using actions::TriggerAction;
using audio::AudioMixer;
using audio::AudioMixerParams;
using utils::Size2d;

const utils::Size2d LoopTake::_Gap = { 6, 6 };

LoopTake::LoopTake(LoopTakeParams params) :
	GuiElement(params),
	MultiAudioSource(),
	_id(params.Id),
	_recordedSampCount(0),
	_loops()
{
}

LoopTake::~LoopTake()
{
}

std::optional<std::shared_ptr<LoopTake>> LoopTake::FromFile(LoopTakeParams takeParams, io::JamFile::LoopTake takeStruct, std::wstring dir)
{
	auto take = std::make_shared<LoopTake>(takeParams);

	LoopParams loopParams;
	loopParams.Wav = "hh";

	for (auto loopStruct : takeStruct.Loops)
	{
		auto loop = Loop::FromFile(loopParams, loopStruct, dir);
		
		if (loop.has_value())
			take->AddLoop(loop.value());
	}

	return take;
}

utils::Position2d LoopTake::Position() const
{
	auto pos = ModelPosition();
	return { (int)round(pos.X), (int)round(pos.Y) };
}

void LoopTake::OnPlay(const std::shared_ptr<MultiAudioSink> dest,
	unsigned int numSamps)
{
	for (auto& loop : _loops)
		loop->OnPlay(dest, numSamps);
}

void LoopTake::EndMultiPlay(unsigned int numSamps)
{
	for (auto& loop : _loops)
		loop->EndMultiPlay(numSamps);
}

void LoopTake::OnWrite(const std::shared_ptr<MultiAudioSource> src,
	unsigned int numSamps)
{
	for (auto& loop : _loops)
	{
		auto inChan = loop->InputChannel();
		src->OnPlayChannel(inChan, loop, numSamps);
	}

	_recordedSampCount += numSamps;
}

void LoopTake::EndMultiWrite(unsigned int numSamps, bool updateIndex)
{
	for (auto& loop : _loops)
		loop->EndWrite(numSamps, updateIndex);
}

void LoopTake::OnPlayRaw(const std::shared_ptr<MultiAudioSink> dest,
	unsigned int delaySamps,
	unsigned int numSamps)
{
	auto loopNum = 0u;
	for (auto& loop : _loops)
	{
		loop->OnPlayRaw(dest, loopNum, delaySamps, numSamps);
		loopNum++;
	}
}

unsigned long LoopTake::Id() const
{
	return _id;
}

unsigned long LoopTake::SourceId() const
{
	return _sourceId;
}

LoopTake::LoopTakeSource LoopTake::SourceType() const
{
	return _sourceType;
}

std::shared_ptr<Loop> LoopTake::AddLoop(unsigned int chan)
{
	auto newNumLoops = (unsigned int)_loops.size() + 1;

	auto loopHeight = CalcLoopHeight(_sizeParams.Size.Height, newNumLoops);

	audio::WireMixBehaviourParams wire;
	wire.Channels = { chan };
	auto mixerParams = Loop::GetMixerParams({ 110, loopHeight }, wire);

	LoopParams loopParams;
	loopParams.Wav = "hh";

	auto loop = std::make_shared<Loop>(loopParams, mixerParams);
	AddLoop(loop);

	return loop;
}

void LoopTake::AddLoop(std::shared_ptr<Loop> loop)
{
	_loops.push_back(loop);
	_children.push_back(loop);
}

void LoopTake::Record(std::vector<unsigned int> channels)
{
	_recordedSampCount = 0;
	_loops.clear();
	auto loopNum = 0;

	for (auto chan : channels)
	{
		auto loop = AddLoop(chan);
		loop->Record();

		_loops.push_back(loop);
		loopNum++;
	}
}

unsigned long LoopTake::NumRecordedSamps() const
{
	return _recordedSampCount;
}

void LoopTake::Play(unsigned long index, unsigned long length)
{
	for (auto& loop : _loops)
	{
		loop->Play(index, length);
	}
}

void LoopTake::Ditch()
{
	_recordedSampCount = 0;

	for (auto& loop : _loops)
	{
		loop->Ditch();
	}

	_loops.clear();
}

unsigned int LoopTake::CalcLoopHeight(unsigned int takeHeight, unsigned int numLoops)
{
	if (0 == numLoops)
		return 0;

	return (takeHeight - ((2 + (numLoops - 1)) * _Gap.Width)) / numLoops;
}

bool LoopTake::_InitResources(ResourceLib& resourceLib)
{
	GuiElement::_InitResources(resourceLib);
	
	ArrangeLoops();

	return false;
}

void LoopTake::ArrangeLoops()
{
	auto numLoops = (unsigned int)_loops.size();

	auto loopHeight = CalcLoopHeight(_sizeParams.Size.Height, numLoops);
	utils::Size2d loopSize = { _sizeParams.Size.Width - (2 * _Gap.Width), _sizeParams.Size.Height - (2 * _Gap.Height) };

	auto loopCount = 0;
	for (auto& loop : _loops)
	{
		loop->SetSize(loopSize);
		loop->SetModelPosition({ (float)_Gap.Width, (float)(_Gap.Height + (loopCount * loopHeight)), 0.0 });

		loopCount++;
	}
}
