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
	_loops({}),
	_backLoops({})
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

// TODO: Remove method
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

void LoopTake::OnWriteChannel(unsigned int channel,
	const std::shared_ptr<base::AudioSource> src,
	unsigned int numSamps)
{
	for (auto& loop : _loops)
	{
		if (loop->InputChannel() == channel)
			src->OnPlay(loop, numSamps);
	}
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
	
	unsigned long highestLoopIndex = 0;
	for (auto& loop : _loops)
	{
		if (loop->Id() > highestLoopIndex)
			highestLoopIndex = loop->Id();
	}

	LoopParams loopParams;
	loopParams.Wav = "hh";
	loopParams.Id = highestLoopIndex + 1;
	auto loop = std::make_shared<Loop>(loopParams, mixerParams);
	AddLoop(loop);

	return loop;
}

void LoopTake::AddLoop(std::shared_ptr<Loop> loop)
{
	_backLoops.push_back(loop);
	_children.push_back(loop);

	ArrangeLoops();
	_resourcesInitialised = false;
	_changesMade = true;
}

void LoopTake::Record(std::vector<unsigned int> channels)
{
	_recordedSampCount = 0;
	_backLoops.clear();

	for (auto chan : channels)
	{
		auto loop = AddLoop(chan);
		loop->Record();
	}

	_changesMade = true;
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

void LoopTake::_InitResources(ResourceLib& resourceLib, bool forceInit)
{
	GuiElement::_InitResources(resourceLib, forceInit);
	
	ArrangeLoops();
}

void LoopTake::_CommitChanges()
{
	std::swap(_backLoops, _loops);
	_backLoops.clear(); // TODO: Undo?
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
