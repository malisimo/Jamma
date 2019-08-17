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
using utils::Size2d;

const utils::Size2d Station::_Gap = { 5, 5 };

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

std::optional<std::shared_ptr<Station>> Station::FromFile(StationParams stationParams, io::JamFile::Station stationStruct)
{
	auto station = std::make_shared<Station>(stationParams);

	auto numTakes = (unsigned int)stationStruct.LoopTakes.size();
	Size2d gap = { 4, 4 };
	auto takeHeight = numTakes > 0 ?
		stationParams.Size.Height - ((2 + numTakes - 1) * gap.Height) / numTakes :
		stationParams.Size.Height - (2 * gap.Height);
	Size2d takeSize = { stationParams.Size.Width - (2 * gap.Width), takeHeight };
	LoopTakeParams takeParams;
	takeParams.Size = { 80, 80 };
	
	auto takeCount = 0u;
	for (auto takeStruct : stationStruct.LoopTakes)
	{
		takeParams.Position = { (int)gap.Width, (int)(takeCount * takeHeight + gap.Height) };
		auto take = LoopTake::FromFile(takeParams, takeStruct);
		
		if (take.has_value())
			station->AddTake(take.value());

		takeCount++;
	}
		
	return station;
}

void Station::OnPlay(const std::shared_ptr<base::MultiAudioSink> dest, unsigned int numSamps)
{
	// Need to rewind buffer as we are pushing to
	// the same place for multiple takes
	for (auto& take : _loopTakes)
		take->OnPlay(dest, numSamps);
}

void Station::EndMultiPlay(unsigned int numSamps)
{
	for (auto& take : _loopTakes)
		take->EndMultiPlay(numSamps);
}

void Station::OnWrite(const std::shared_ptr<base::MultiAudioSource> src, unsigned int numSamps)
{
	for (auto& take : _loopTakes)
		take->OnWrite(src, numSamps);
}

void Station::EndMultiWrite(unsigned int numSamps, bool updateIndex)
{
	for (auto& take : _loopTakes)
		take->EndMultiWrite(numSamps, updateIndex);
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

	LoopTakeParams takeParams;
	auto loopCount = 0u;
	auto loopTake = TryGetTake(action.TargetId);

	switch (action.ActionType)
	{
	case TriggerAction::TRIGGER_REC_START:
		if (loopTake.has_value())
			loopTake.value()->Record(action.InputChannels);

		res.IsEaten = true;
		break;
	case TriggerAction::TRIGGER_REC_END:
		if (_globalClock->IsMasterLengthSet())
		{
			auto [loopLength, errorSamps] = _globalClock->QuantiseLength(action.SampleCount);
			
			if (loopTake.has_value())
				loopTake.value()->Play(0, loopLength);
		}
		else
			_globalClock->SetMasterLength(action.SampleCount);

		res.IsEaten = true;
		break;
	case TriggerAction::TRIGGER_DITCH:
		if (loopTake.has_value())
			loopTake.value()->Ditch();

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

std::shared_ptr<LoopTake> Station::AddTake()
{
	auto newNumTakes = (unsigned int)_loopTakes.size() + 1;

	auto takeHeight = CalcTakeHeight(_sizeParams.Size.Height, newNumTakes);
	utils::Size2d takeSize = { _sizeParams.Size.Width - (2 * _Gap.Width), _sizeParams.Size.Height - (2 * _Gap.Height) };

	auto takeCount = 0;
	for (auto& take : _loopTakes)
	{
		take->SetSize(takeSize);
		take->SetPosition({ (int)_Gap.Width, (int)(_Gap.Height + (takeCount * takeHeight)) });

		takeCount++;
	}

	LoopTakeParams takeParams;
	takeParams.Size = takeSize;
	takeParams.Position = { (int)_Gap.Width, (int)(_Gap.Height + (newNumTakes - 1) * takeHeight) };

	auto take = std::make_shared<LoopTake>(takeParams);

	AddTake(take);

	return take;
}

void Station::AddTake(std::shared_ptr<LoopTake> take)
{
	_loopTakes.push_back(take);
	_children.push_back(take);
}

void Station::AddTrigger(std::shared_ptr<Trigger> trigger)
{
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


unsigned int Station::CalcTakeHeight(unsigned int stationHeight, unsigned int numTakes)
{
	if (0 == numTakes)
		return 0;

	return (stationHeight - ((2 + (numTakes - 1)) * _Gap.Width)) / numTakes;
}


std::optional<std::shared_ptr<LoopTake>> Station::TryGetTake(unsigned long id)
{
	for (auto& take : _loopTakes)
	{
		if (take->Id() == id)
			return take;
	}

	return std::nullopt;
}
