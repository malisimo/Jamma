#pragma once

#include <vector>
#include <optional>
#include "ActionReceiver.h"
#include "../actions/KeyAction.h"
#include "../actions/TriggerAction.h"
#include "../actions/ActionResult.h"

namespace engine
{
	enum TriggerSource
	{
		TRIGGER_NOTSET,
		TRIGGER_KEY,
		TRIGGER_MIDI
	};

	class TriggerBinding
	{
	public:
		TriggerBinding() :
			TriggerSource(TRIGGER_NOTSET),
			Value(0),
			State(0)
		{
		}

		TriggerBinding(TriggerSource source,
			unsigned int value,
			unsigned int state) :
			TriggerSource(source),
			Value(value),
			State(state)
		{
		}

	public:
		bool Test(TriggerSource source,
			unsigned int value,
			unsigned int state)
		{
			if ((TRIGGER_NOTSET != TriggerSource) &&
				(source == TriggerSource) &&
				(value == Value) &&
				(state == State))
				return true;

			return false;
		}

	public:
		TriggerSource TriggerSource;
		unsigned int Value;
		unsigned int State;
	};

	class DualBinding
	{
	public:
		enum TestResult
		{
			MATCH_NONE,
			MATCH_DOWN,
			MATCH_RELEASE
		};

	public:
		DualBinding() :
			_isDown(false)
		{
		};

	public:
		void SetDown(TriggerBinding binding,
			bool inferRelease)
		{
			_triggerDown = binding;

			if (inferRelease)
			{
				_triggerRelease = TriggerBinding(_triggerDown.TriggerSource,
					_triggerDown.Value,
					_triggerDown.State > 0 ? 0 : 1);
			}
		}

		void SetRelease(TriggerBinding binding,
			bool inferDown)
		{
			_triggerRelease = binding;

			if (inferDown && _triggerRelease.has_value())
			{
				auto trigRelease = _triggerRelease.value();
				_triggerDown = TriggerBinding(trigRelease.TriggerSource,
					trigRelease.Value,
					trigRelease.State > 0 ? 0 : 1);
			}
		}

		TestResult OnTrigger(TriggerSource source,
			unsigned int value,
			unsigned int state)
		{
			if (!_isDown)
			{
				if (_triggerDown.Test(source, value, state))
				{
					if (_triggerRelease.has_value())
						_isDown = true;

					return MATCH_DOWN;
				}
			}
			else
			{
				if (_triggerRelease.has_value())
				{
					auto trigRelease = _triggerRelease.value();
					if (trigRelease.Test(source, value, state))
					{
						_isDown = false;
						return MATCH_RELEASE;
					}
				}
			}

			return MATCH_NONE;
		}

		void Reset() { _isDown = false;	}

	private:
		TriggerBinding _triggerDown;
		std::optional<TriggerBinding> _triggerRelease;
		bool _isDown;
	};

	struct TriggerParams
	{
		std::vector<DualBinding> Bindings;
	};

	enum TriggerState
	{
		TRIGGER_DEFAULT,
		TRIGGER_RECORDING,
		TRIGGER_DITCHDOWN,
		TRIGGER_ARMED,
		TRIGGER_OVERDUBBING
	};

	class Trigger :
		public base::ActionReceiver,
		public base::ActionSender
	{
	public:
		Trigger();
		Trigger(TriggerParams activateParams,
			TriggerParams ditchParams);
		~Trigger();

	public:
		virtual actions::ActionResult OnAction(actions::KeyAction action) override;

		bool TryChangeState(DualBinding& binding,
			bool isActivate,
			const actions::KeyAction& action,
			int keyState);
		
		TriggerState GetState() const;
		void Reset();

	private:
		bool StateMachine(bool isDown, bool isActivate);

	private:
		TriggerParams _activateParams;
		TriggerParams _ditchParams;
		TriggerState _state;
	};
}
