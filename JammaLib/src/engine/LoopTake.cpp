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

void LoopTake::OnPlay(const std::shared_ptr<MultiAudioSink> dest, unsigned int numSamps)
{
	for (auto& loop : _loops)
		loop->OnPlay(dest, numSamps);
}

/*void LoopTake::OnWrite(const std::shared_ptr<MultiAudioSource> src, unsigned int numSamps)
{
	for (auto& loop : _loops)
	{
		auto inChan = loop->InputChannel();
		src->OnPlayChannel(inChan, loop, numSamps);
	}

	_recordedSampCount += numSamps;
}*/

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

void LoopTake::AddLoop(LoopParams loopParams)
{
	auto loop = std::make_shared<Loop>(loopParams);
	_loops.push_back(loop);
	_children.push_back(loop);
}

void LoopTake::Record(std::vector<unsigned int> channels)
{
	_recordedSampCount = 0;
	_loops.clear();
	auto loopNum = 0;

	for (auto& chan : channels)
	{
		gui::GuiSliderParams sliderParams;
		sliderParams.Position = { 2,4 };
		sliderParams.Size = { 40,312 };
		sliderParams.MinSize = { 40,312 };
		sliderParams.DragLength = 270;
		sliderParams.DragControlOffset = { 4,5 };
		sliderParams.DragControlSize = { 32,32 };
		sliderParams.Texture = "fader_back";
		sliderParams.DragTexture = "fader";
		sliderParams.DragOverTexture = "fader_over";

		audio::WireMixBehaviourParams bParams;
		bParams.Channels = { chan };
		AudioMixerParams mixerParams;
		mixerParams.Size = { 160, 320 };
		mixerParams.Position = { 6, 6 };
		mixerParams.SliderParams = sliderParams;
		mixerParams.Behaviour = bParams;

		LoopParams loopParams;
		loopParams.Size = { 110, 80 };
		loopParams.Position = { 5, 5 + loopNum * 90 };
		loopParams.MixerParams = mixerParams;

		auto loop = std::make_shared<Loop>(loopParams);
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
