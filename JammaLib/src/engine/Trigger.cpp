#include "Trigger.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

using namespace base;
using namespace engine;
using actions::ActionResult;
using actions::KeyAction;
using actions::TriggerAction;
using graphics::GlDrawContext;
using graphics::Image;
using graphics::ImageParams;
using resources::ResourceLib;

Trigger::Trigger(TriggerParams trigParams) :
	GuiElement(trigParams),
	_activateBindings(trigParams.Activate),
	_ditchBindings(trigParams.Ditch),
	_state(TRIGSTATE_DEFAULT),
	_debounceTimeMs(trigParams.DebounceMs),
	_lastActivateTime(),
	_lastDitchTime(),
	_isLastActivateDown(false),
	_isLastDitchDown(false),
	_isLastActivateDownRaw(false),
	_isLastDitchDownRaw(false),
	_textureRecording(ImageParams(DrawableParams{ trigParams.TextureRecording }, SizeableParams{ trigParams.Size,trigParams.MinSize }, "texture")),
	_textureDitchDown(ImageParams(DrawableParams{ trigParams.TextureDitchDown }, SizeableParams{ trigParams.Size,trigParams.MinSize }, "texture")),
	_textureOverdubbing(ImageParams(DrawableParams{ trigParams.TextureOverdubbing }, SizeableParams{ trigParams.Size,trigParams.MinSize }, "texture")),
	_texturePunchedIn(ImageParams(DrawableParams{ trigParams.TexturePunchedIn }, SizeableParams{ trigParams.Size,trigParams.MinSize }, "texture"))
{
}

Trigger::~Trigger()
{
}


std::optional<std::shared_ptr<Trigger>> Trigger::FromFile(TriggerParams trigParams, io::RigFile::Trigger trigStruct)
{
	auto trigger = std::make_shared<Trigger>(trigParams);

	for (auto trigPair : trigStruct.TriggerPairs)
	{
		auto activate = DualBinding(
			TriggerBinding{
				TriggerSource::TRIGGER_KEY,
				trigPair.ActivateDown,
				1
			},
			TriggerBinding{
				TriggerSource::TRIGGER_KEY,
				trigPair.ActivateUp,
				0
			});

		auto ditch = DualBinding(
			TriggerBinding{
				TriggerSource::TRIGGER_KEY,
				trigPair.DitchDown,
				1
			},
			TriggerBinding{
				TriggerSource::TRIGGER_KEY,
				trigPair.DitchUp,
				0
			});

		trigger->AddBinding(activate, ditch);
	}

	return trigger;
}

utils::Position2d Trigger::Position() const
{
	auto pos = ModelPosition();
	return { (int)round(pos.X), (int)round(pos.Y) };
}

ActionResult Trigger::OnAction(KeyAction action)
{
	ActionResult res;
	res.IsEaten = false;
	res.ResultType = actions::ACTIONRESULT_DEFAULT;

	auto keyState = KeyAction::KEY_DOWN == action.KeyActionType ? 1 : 0;

	for (auto& b : _activateBindings)
	{
		if (TryChangeState(b, true, action, keyState))
		{
			res.IsEaten = true;
			return res;
		}
	}
	for (auto& b : _ditchBindings)
	{
		if (TryChangeState(b, false, action, keyState))
		{
			res.IsEaten = true;
			return res;
		}
	}

	return res;
}

void Trigger::OnTick(Time curTime, unsigned int samps)
{
	if ((TriggerState::TRIGSTATE_DEFAULT != _state) &&
		(TriggerState::TRIGSTATE_DITCHDOWN != _state))
		_recordSampCount++;

	if (0 == _debounceTimeMs)
		return;

	// Eventually flick to new state
	// (if held long enough)
	auto elapsedMs = Timer::GetElapsedSeconds(_lastActivateTime, curTime) * 1000.0;

	if (_isLastActivateDownRaw != _isLastActivateDown)
	{
		if (elapsedMs > _debounceTimeMs)
		{
			_lastActivateTime = curTime;
			_isLastActivateDown = _isLastActivateDownRaw;

			StateMachine(_isLastActivateDownRaw, true);
		}
	}
	
	elapsedMs = Timer::GetElapsedSeconds(_lastDitchTime, curTime) * 1000.0;
	
	if (_isLastDitchDownRaw != _isLastDitchDown)
	{
		if (elapsedMs > _debounceTimeMs)
		{
			_lastDitchTime = curTime;
			_isLastDitchDown = _isLastDitchDownRaw;

			StateMachine(_isLastDitchDownRaw, false);
		}
	}
}

void Trigger::Draw(base::DrawContext& ctx)
{
	if (TRIGSTATE_DEFAULT == _state)
	{
		GuiElement::Draw(ctx);
		return;
	}

	auto& glCtx = dynamic_cast<GlDrawContext&>(ctx);
	auto pos = Position();
	glCtx.PushMvp(glm::translate(glm::mat4(1.0), glm::vec3(pos.X, pos.Y, 0.f)));

	switch (_state)
	{
	case TRIGSTATE_RECORDING:
		_textureRecording.Draw(ctx);
		break;
	case TRIGSTATE_DITCHDOWN:
		_textureDitchDown.Draw(ctx);
		break;
	case TRIGSTATE_OVERDUBBING:
		_textureOverdubbing.Draw(ctx);
		break;
	case TRIGSTATE_PUNCHEDIN:
		_texturePunchedIn.Draw(ctx);
		break;
	}

	for (auto& child : _children)
		child->Draw(ctx);

	glCtx.PopMvp();
}

void Trigger::AddBinding(DualBinding activate, DualBinding ditch)
{
	_activateBindings.push_back(activate);
	_ditchBindings.push_back(ditch);
}

TriggerState Trigger::GetState() const
{
	return _state;
}

void Trigger::Reset()
{
	_state = TRIGSTATE_DEFAULT;
	
	for (auto& b : _activateBindings)
		b.Reset();

	for (auto& b : _ditchBindings)
		b.Reset();

	_state = TriggerState::TRIGSTATE_DEFAULT;
	_targetId = 0;
	_overdubTargetId = 0;
	_recordSampCount = 0;
}

bool Trigger::IgnoreRepeats(bool isActivate, DualBinding::TestResult trigResult)
{
	bool allowedThrough = true;

	if (isActivate)
	{
		if ((DualBinding::MATCH_DOWN == trigResult) && _isLastActivateDownRaw)
			allowedThrough = false;
		else if ((DualBinding::MATCH_RELEASE == trigResult) && !_isLastActivateDownRaw)
			allowedThrough = false;
	}
	else
	{
		if ((DualBinding::MATCH_DOWN == trigResult) && _isLastDitchDownRaw)
			allowedThrough = false;
		else if ((DualBinding::MATCH_RELEASE == trigResult) && !_isLastDitchDownRaw)
			allowedThrough = false;
	}

	return allowedThrough;
}

bool Trigger::Debounce(bool isActivate,
	DualBinding::TestResult trigResult,
	Time actionTime)
{
	auto allowedThrough = false;

	if (isActivate)
	{
		auto elapsedMs = Timer::GetElapsedSeconds(_lastActivateTime, actionTime) * 1000.0;

		if ((DualBinding::MATCH_DOWN == trigResult) && !_isLastActivateDownRaw)
		{
			_lastActivateTime = actionTime;
			_isLastActivateDownRaw = true;

			if ((0 == _debounceTimeMs) || (elapsedMs > _debounceTimeMs))
			{
				allowedThrough = true;
				_isLastActivateDown = true;
			}
		}
		else if ((DualBinding::MATCH_RELEASE == trigResult) && _isLastActivateDownRaw)
		{
			_lastActivateTime = actionTime;
			_isLastActivateDownRaw = false;

			if ((0 == _debounceTimeMs) || (elapsedMs > _debounceTimeMs))
			{
				allowedThrough = true;
				_isLastActivateDown = false;
			}
		}
	}
	else
	{
		auto elapsedMs = Timer::GetElapsedSeconds(_lastDitchTime, actionTime) * 1000.0;

		if ((DualBinding::MATCH_DOWN == trigResult) && !_isLastDitchDownRaw)
		{
			_lastDitchTime = actionTime;
			_isLastDitchDownRaw = true;

			if ((0 == _debounceTimeMs) || (elapsedMs > _debounceTimeMs))
			{
				allowedThrough = true;
				_isLastDitchDown = true;
			}
		}
		else if ((DualBinding::MATCH_RELEASE == trigResult) && _isLastDitchDownRaw)
		{
			_lastDitchTime = actionTime;
			_isLastDitchDownRaw = false;

			if ((0 == _debounceTimeMs) || (elapsedMs > _debounceTimeMs))
			{
				allowedThrough = true;
				_isLastDitchDown = false;
			}
		}
	}

	return allowedThrough;
}

bool Trigger::TryChangeState(DualBinding& binding,
	bool isActivate,
	const actions::KeyAction& action,
	int keyState)
{
	auto trigResult = binding.OnTrigger(
		TriggerSource::TRIGGER_KEY,
		action.KeyChar,
		keyState);

	bool allowedThrough = IgnoreRepeats(isActivate, trigResult);
	if (!allowedThrough)
		return false;

	allowedThrough = Debounce(isActivate,
		trigResult,
		action.GetActionTime());

	if (!allowedThrough)
		return false;

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

bool Trigger::StateMachine(bool isDown, bool isActivate)
{
	bool changedState = false;

	switch (_state)
	{
	case TRIGSTATE_DEFAULT:
		if (isDown)
		{
			if (isActivate)
			{
				StartRecording();
				changedState = true;
			}
			else
			{
				SetDitchDown();
				changedState = true;
			}
		}
		break;
	case TRIGSTATE_RECORDING:
		if (isActivate && isDown)
		{
			EndRecording();
			changedState = true;
		}
		else if (!isActivate && isDown)
		{
			Ditch();
			changedState = true;
		}
		break;
	case TRIGSTATE_DITCHDOWN:
		if (isActivate && isDown)
		{
			StartOverdub();
			changedState = true;
		}
		else if (!isActivate && !isDown)
		{
			Ditch();
			changedState = true;
		}
		break;
	case TRIGSTATE_OVERDUBBING:
		if (isActivate && isDown)
		{
			EndOverdub();
			changedState = true;
		}
		else if (!isActivate && isDown)
		{
			StartPunchIn();
			changedState = true;
		}
		break;
	case TRIGSTATE_PUNCHEDIN:
		if (isActivate && isDown)
		{
			DitchOverdub();
			changedState = true;
		}
		if (!isActivate && !isDown)
		{
			// End punch-in but maintain overdub mode (release)
			EndPunchIn();
			changedState = true;
		}
		break;
	}

	return changedState;
}

void Trigger::StartRecording()
{
	_state = TRIGSTATE_RECORDING;
	_recordSampCount = 0;

	if (_receiver)
	{
		TriggerAction trigAction;
		trigAction.ActionType = TriggerAction::TRIGGER_REC_START;
		auto res = _receiver->OnAction(trigAction);

		if (res.IsEaten)
			_targetId = res.Id;
	}
}

void Trigger::EndRecording()
{
	_state = TRIGSTATE_DEFAULT;

	if (_receiver)
	{
		TriggerAction trigAction;
		trigAction.ActionType = TriggerAction::TRIGGER_REC_END;
		trigAction.TargetId = _targetId;
		trigAction.SampleCount = _recordSampCount;
		_receiver->OnAction(trigAction);
	}
}

void Trigger::SetDitchDown()
{
	_state = TRIGSTATE_DITCHDOWN;
}

void Trigger::Ditch()
{
	_state = TRIGSTATE_DEFAULT;

	if (_receiver)
	{
		TriggerAction trigAction;
		trigAction.ActionType = TriggerAction::TRIGGER_DITCH;
		trigAction.TargetId = _targetId;
		trigAction.SampleCount = _recordSampCount;
		_receiver->OnAction(trigAction);
	}

	_targetId = 0;
	_overdubTargetId = 0;
}

void Trigger::StartOverdub()
{
	_state = TRIGSTATE_OVERDUBBING;
	_recordSampCount = 0;

	if (_receiver)
	{
		TriggerAction trigAction;
		trigAction.ActionType = TriggerAction::TRIGGER_OVERDUB_START;
		trigAction.SampleCount = _recordSampCount;
		auto res = _receiver->OnAction(trigAction);

		if (res.IsEaten)
			_overdubTargetId = res.Id;
	}
}

void Trigger::EndOverdub()
{
	_state = TRIGSTATE_DEFAULT;

	if (_receiver)
	{
		TriggerAction trigAction;
		trigAction.ActionType = TriggerAction::TRIGGER_OVERDUB_END;
		trigAction.TargetId = _overdubTargetId;
		trigAction.SampleCount = _recordSampCount;
		_receiver->OnAction(trigAction);
	}
}

void Trigger::DitchOverdub()
{
	_state = TRIGSTATE_DEFAULT;

	if (_receiver)
	{
		TriggerAction trigAction;
		trigAction.ActionType = TriggerAction::TRIGGER_OVERDUB_DITCH;
		trigAction.TargetId = _overdubTargetId;
		trigAction.SampleCount = _recordSampCount;
		_receiver->OnAction(trigAction);
	}

	_overdubTargetId = 0;
}

void Trigger::StartPunchIn()
{
	_state = TRIGSTATE_PUNCHEDIN;

	if (_receiver)
	{
		TriggerAction trigAction;
		trigAction.ActionType = TriggerAction::TRIGGER_PUNCHIN_START;
		trigAction.TargetId = _overdubTargetId;
		trigAction.SampleCount = _recordSampCount;
		_receiver->OnAction(trigAction);
	}
}

void Trigger::EndPunchIn()
{
	_state = TRIGSTATE_OVERDUBBING;

	if (_receiver)
	{
		TriggerAction trigAction;
		trigAction.ActionType = TriggerAction::TRIGGER_PUNCHIN_END;
		trigAction.TargetId = _overdubTargetId;
		trigAction.SampleCount = _recordSampCount;
		_receiver->OnAction(trigAction);
	}
}

bool Trigger::_InitResources(ResourceLib& resourceLib)
{
	_textureRecording.InitResources(resourceLib);
	_textureDitchDown.InitResources(resourceLib);
	_textureOverdubbing.InitResources(resourceLib);
	_texturePunchedIn.InitResources(resourceLib);

	return GuiElement::_InitResources(resourceLib);
}

bool Trigger::_ReleaseResources()
{
	auto res = GuiElement::_ReleaseResources();

	_textureRecording.ReleaseResources();
	_textureDitchDown.ReleaseResources();
	_textureOverdubbing.ReleaseResources();
	_texturePunchedIn.ReleaseResources();

	return res;
}