#include "Station.h"

using namespace engine;
using base::MultiAudioSource;
using base::AudioSink;
using base::GuiElement;
using base::GuiElementParams;
using base::DrawContext;
using resources::ResourceLib;
using actions::ActionResult;
using actions::KeyAction;
using actions::TouchAction;
using actions::TriggerAction;
using gui::GuiSliderParams;
using audio::AudioMixerParams;
using audio::WireMixBehaviourParams;

Station::Station(StationParams params) :
	GuiElement(params),
	MultiAudioSource(),
	_globalClock(params.GlobalClock),
	_loopTakes(),
	_triggers({})
{
}

Station::~Station()
{
}

void Station::OnPlay(const std::shared_ptr<base::MultiAudioSink> dest, unsigned int numSamps)
{
	// Need to rewind buffer as we are pushing to
	// the same place for multiple takes
	for (auto& take : _loopTakes)
		take->OnPlay(dest, numSamps);
}

void Station::OnWrite(const std::shared_ptr<base::MultiAudioSource> src, unsigned int numSamps)
{
	for (auto& take : _loopTakes)
		take->OnWrite(src, numSamps);
}

void Station::Offset(unsigned int numSamps)
{
	for (auto& take : _loopTakes)
		take->Offset(numSamps);
}

ActionResult Station::OnAction(KeyAction action)
{
	ActionResult res;
	res.IsEaten = false;
	res.ResultType = actions::ACTIONRESULT_DEFAULT;

	for (auto& trig : _triggers)
	{
		auto trigResult = trig->OnAction(action);
		if (trigResult.IsEaten)
			return trigResult;
	}

	res.IsEaten = false;
	return res;
}

ActionResult Station::OnAction(TouchAction action)
{
	return GuiElement::OnAction(action);
}

ActionResult Station::OnAction(TriggerAction action)
{
	ActionResult res;
	res.IsEaten = false;

	std::shared_ptr<LoopTake> loopTake = nullptr;

	switch (action.ActionType)
	{
	case TriggerAction::TRIGGER_REC_START:
		loopTake->Record(action.InputChannels);
		res.IsEaten = true;
		break;
	case TriggerAction::TRIGGER_REC_END:
		loopTake = AddLoopTake(action);
		if (_globalClock->IsMasterLengthSet())
		{
			auto [loopLength, errorSamps] = _globalClock->QuantiseLength(action.SampleCount);
			loopTake->Play(0, loopLength);
		}
		else
		{
			_globalClock->SetMasterLength(action.SampleCount);
		}
		res.IsEaten = true;
		break;
	case TriggerAction::TRIGGER_DITCH:
		loopTake->Ditch();
		res.IsEaten = true;
		break;
	}

	return res;
}

void Station::OnTick(Time curTime, unsigned int samps)
{
	for (auto& trig : _triggers)
	{
		trig->OnTick(curTime, samps);
	}
}

void Station::AddTake(LoopTakeParams takeParams)
{
	auto take = std::make_shared<LoopTake>(takeParams);

	for (auto loop : takeParams.Loops)
		take->AddLoop(loop);

	_loopTakes.push_back(take);
	_children.push_back(take);
}

void Station::AddTrigger(TriggerParams trigParams)
{
	auto trigger = std::make_shared<Trigger>(trigParams);

	_triggers.push_back(trigger);
	_children.push_back(trigger);
}

void Station::Reset()
{
	for (auto take : _loopTakes)
	{
		auto child = std::find(_children.begin(), _children.end(), take);
		if (_children.end() != child)
			_children.erase(child);
	}
	_loopTakes.clear();

	for (auto trigger : _triggers)
	{
		auto child = std::find(_children.begin(), _children.end(), trigger);
		if (_children.end() != child)
			_children.erase(child);
	}
	_triggers.clear();
}

std::shared_ptr<LoopTake> engine::Station::GetLoopTake(unsigned long id)
{
	for (auto& take : _loopTakes)
	{
		if (take->Id() == id)
			return take;
	}

	return std::shared_ptr<LoopTake>();
}

std::shared_ptr<LoopTake> Station::AddLoopTake(TriggerAction action)
{
	// Nicer with default constructor
	GuiSliderParams sliderParams;
	sliderParams.Position = { 2,4 };
	sliderParams.Size = { 40,312 };
	sliderParams.MinSize = { 40,312 };
	sliderParams.DragLength = 270;
	sliderParams.DragControlOffset = { 4,5 };
	sliderParams.DragControlSize = { 32,32 };
	sliderParams.Texture = "fader_back";
	sliderParams.DragTexture = "fader";
	sliderParams.DragOverTexture = "fader_over";

	WireMixBehaviourParams mixBehaviour;
	mixBehaviour.Channels = { 0 };

	std::vector<LoopParams> takeLoops;
	for (auto chan : action.InputChannels)
	{
		AudioMixerParams mixerParams;
		mixerParams.Size = { 160, 320 };
		mixerParams.Position = { 6, 6 };
		mixerParams.SliderParams = sliderParams;
		mixerParams.Behaviour = mixBehaviour;
		mixerParams.InputChannel = chan;

		LoopParams loopParams;
		loopParams.Size = { 80, 80 };
		loopParams.Position = { 10, 22 };
		loopParams.MixerParams = mixerParams;

		takeLoops.push_back(loopParams);
	}

	LoopTakeParams takeParams;
	takeParams.Size = { 140, 140 };
	takeParams.Position = { 4, 4 };
	takeParams.Loops = takeLoops;
	
	auto take = std::make_shared<LoopTake>(takeParams);

	for (auto loop : takeParams.Loops)
		take->AddLoop(loop);

	return take;
}
