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

Station::Station(StationParams params) :
	GuiElement(params),
	MultiAudioSource(),
	_loopTakes(),
	_triggers({})
{
}

Station::~Station()
{
}

void Station::Play(const std::vector<std::shared_ptr<base::AudioSink>>& dest, unsigned int numSamps)
{
	// Need to rewind buffer as we are pushing to the same place for multiple takes
	for (auto& take : _loopTakes)
		take->Play(dest, numSamps);
}

ActionResult Station::OnAction(KeyAction action)
{
	ActionResult res;
	res.IsEaten = false;
	res.ResultType = actions::ACTIONRESULT_DEFAULT;

	for (auto trig : _triggers)
	{
		auto trigResult = trig.OnAction(action);
		if (trigResult.IsEaten)
			return trigResult;
	}

	res.IsEaten = false;
	return res;
}

ActionResult Station::OnAction(TouchAction action)
{
	return ActionReceiver::OnAction(action);
}

void Station::AddTake(LoopTakeParams takeParams)
{
	auto take = std::make_shared<LoopTake>(takeParams);

	for (auto loop : takeParams.Loops)
		take->AddLoop(loop);

	_loopTakes.push_back(take);
	_children.push_back(take);
}