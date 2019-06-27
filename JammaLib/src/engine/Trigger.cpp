#include "Trigger.h"

using namespace engine;
using actions::ActionResult;
using actions::KeyAction;
using actions::TriggerAction;

Trigger::Trigger() :
	_state(TRIGGER_DEFAULT)
{
}

Trigger::Trigger(TriggerParams activateParams,
	TriggerParams ditchParams) :
	_activateParams(activateParams),
	_ditchParams(ditchParams),
	_state(TRIGGER_DEFAULT)
{
}

Trigger::~Trigger()
{
}

ActionResult engine::Trigger::OnAction(KeyAction action)
{
	ActionResult res;
	res.IsEaten = false;
	res.ResultType = actions::ACTIONRESULT_DEFAULT;

	auto keyState = KeyAction::KEY_DOWN == action.KeyActionType ? 1 : 0;

	for (auto& b : _activateParams.Bindings)
	{
		if (TryChangeState(b, true, action, keyState))
		{
			res.IsEaten = true;
			return res;
		}
	}
	for (auto& b : _ditchParams.Bindings)
	{
		if (TryChangeState(b, false, action, keyState))
		{
			res.IsEaten = true;
			return res;
		}
	}

	return res;
}

bool Trigger::TryChangeState(engine::DualBinding& binding,
	bool isActivate,
	const actions::KeyAction& action,
	int keyState)
{
	auto trigResult = binding.OnTrigger(TriggerSource::TRIGGER_KEY,
		action.KeyChar,
		keyState);

	switch (trigResult)
	{
	case DualBinding::MATCH_DOWN:
		StateMachine(true, isActivate);
		return true;
	case DualBinding::MATCH_RELEASE:
		StateMachine(false, isActivate);
		return true;
	}

	return false;
}

TriggerState Trigger::GetState() const
{
	return _state;
}

void Trigger::Reset()
{
	_state = TRIGGER_DEFAULT;
	
	for (auto& b : _activateParams.Bindings)
	{
		b.Reset();
	}
	for (auto& b : _ditchParams.Bindings)
	{
		b.Reset();
	}
}

bool Trigger::StateMachine(bool isDown, bool isActivate)
{
	bool changedState = false;

	switch (_state)
	{
	case TRIGGER_DEFAULT:
		if (isDown)
		{
			if (isActivate)
			{
				// Start recording (on press)
				_state = TRIGGER_RECORDING;
				changedState = true;

				TriggerAction trigAction;
				trigAction.ActionType = TriggerAction::TRIGGER_REC_START;
				_receiver->OnAction(trigAction);
			}
			else
			{
				// Ditch is held down
				_state = TRIGGER_DITCHDOWN;
				changedState = true;
			}
		}
		break;
	case TRIGGER_RECORDING:
		if (isActivate && isDown)
		{
			// End recording (on press)
			_state = TRIGGER_DEFAULT;
			changedState = true;

			TriggerAction trigAction;
			trigAction.ActionType = TriggerAction::TRIGGER_REC_END;
			_receiver->OnAction(trigAction);
		}
		break;
	case TRIGGER_DITCHDOWN:
		if (isActivate && isDown)
		{
			// Start armed mode (on press)
			_state = TRIGGER_ARMED;
			changedState = true;

			TriggerAction trigAction;
			trigAction.ActionType = TriggerAction::TRIGGER_ARM_START;
			_receiver->OnAction(trigAction);
		}
		else if (!isActivate && !isDown)
		{
			// Ditch (on release)
			_state = TRIGGER_DEFAULT;
			changedState = true;

			TriggerAction trigAction;
			trigAction.ActionType = TriggerAction::TRIGGER_DITCH;
			_receiver->OnAction(trigAction);
		}
		break;
	case TRIGGER_ARMED:
		if (isActivate && isDown)
		{
			// End arm and overdub (on press)
			_state = TRIGGER_DEFAULT;
			changedState = true;

			TriggerAction trigAction;
			trigAction.ActionType = TriggerAction::TRIGGER_ARM_END;
			_receiver->OnAction(trigAction);
		}
		else if (!isActivate && isDown)
		{
			// Start overdubbing (on press)
			_state = TRIGGER_OVERDUBBING;
			changedState = true;

			TriggerAction trigAction;
			trigAction.ActionType = TriggerAction::TRIGGER_OVERDUB_START;
			_receiver->OnAction(trigAction);
		}
		break;
	case TRIGGER_OVERDUBBING:
		if (!isActivate && !isDown)
		{
			// End overdubbing but maintain arm (on release)
			_state = TRIGGER_ARMED;
			changedState = true;

			TriggerAction trigAction;
			trigAction.ActionType = TriggerAction::TRIGGER_OVERDUB_END;
			_receiver->OnAction(trigAction);
		}
		break;
	}

	return changedState;
}