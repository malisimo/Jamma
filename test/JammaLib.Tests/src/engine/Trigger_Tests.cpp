
#include "gtest/gtest.h"
#include "resources/ResourceLib.h"
#include "engine/Trigger.h"

using base::ActionSender;
using base::ActionReceiver;
using engine::Trigger;
using engine::TriggerParams;
using actions::TriggerAction;
using actions::KeyAction;

const unsigned int ActivateChar = 49;
const unsigned int DitchChar = 50;
const unsigned int ActivateMidi = 211;
const unsigned int DitchMidi = 212;

class MockedTriggerReceiver :
	public ActionReceiver
{
public:
	MockedTriggerReceiver() :
		ActionReceiver(),
		_expected(TriggerAction::TRIGGER_REC_START),
		_lastMatched(false),
		_numTimesCalled(0) {}
	MockedTriggerReceiver(TriggerAction::TriggerActionType expected) :
		ActionReceiver(),
		_expected(expected),
		_lastMatched(false),
		_numTimesCalled(0) {}
public:
	virtual actions::ActionResult OnAction(actions::TriggerAction action)
	{
		_numTimesCalled++;
		_lastMatched = action.ActionType == _expected;
		return { _lastMatched, actions::ACTIONRESULT_DEFAULT };
	};
	void SetExpected(TriggerAction::TriggerActionType expected) { _expected = expected; }
	bool GetLastMatched() const { return _lastMatched; }
	int GetNumTimesCalled() const { return _numTimesCalled; }

private:
	TriggerAction::TriggerActionType _expected;
	bool _lastMatched;
	int _numTimesCalled;
};

Trigger MakeDefaultTrigger(std::shared_ptr<MockedTriggerReceiver> receiver)
{
	auto activateBind = engine::DualBinding();
	activateBind.SetDown(engine::TriggerBinding(engine::TRIGGER_KEY, ActivateChar, 1), true);

	auto ditchBind = engine::DualBinding();
	ditchBind.SetRelease(engine::TriggerBinding(engine::TRIGGER_KEY, DitchChar, 0), true);

	TriggerParams activateParams;
	activateParams.Bindings = { activateBind };
	TriggerParams ditchParams;
	ditchParams.Bindings = { ditchBind };
	auto trigger = Trigger(activateParams, ditchParams);
	trigger.SetReceiver(receiver);

	return trigger;
}

TEST(Trigger, DitchesLoop) {
	auto receiver = std::make_shared<MockedTriggerReceiver>();
	auto trigger = MakeDefaultTrigger(receiver);
	auto action = KeyAction();

	receiver->SetExpected(TriggerAction::TRIGGER_REC_START);
	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	trigger.OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());

	receiver->SetExpected(TriggerAction::TRIGGER_DITCH);
	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_UP;
	trigger.OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
}

TEST(Trigger, RecordsTwoLoops) {
	auto receiver = std::make_shared<MockedTriggerReceiver>();
	auto trigger = MakeDefaultTrigger(receiver);
	auto action = KeyAction();

	receiver->SetExpected(TriggerAction::TRIGGER_REC_START);
	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	trigger.OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(1, receiver->GetNumTimesCalled());

	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_UP;
	trigger.OnAction(action);
	ASSERT_EQ(1, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_REC_END);
	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	trigger.OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(2, receiver->GetNumTimesCalled());

	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_UP;
	trigger.OnAction(action);
	ASSERT_EQ(2, receiver->GetNumTimesCalled());
}

TEST(Trigger, NoReleaseSkipsAction) {
	auto receiver = std::make_shared<MockedTriggerReceiver>();
	auto trigger = MakeDefaultTrigger(receiver);
	auto action = KeyAction();

	receiver->SetExpected(TriggerAction::TRIGGER_REC_START);
	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	trigger.OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());

	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	trigger.OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(1, receiver->GetNumTimesCalled());
}

TEST(Trigger, OverDubReleasingActivateFirst) {
	auto receiver = std::make_shared<MockedTriggerReceiver>();
	auto trigger = MakeDefaultTrigger(receiver);
	auto action = KeyAction();

	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	trigger.OnAction(action);
	ASSERT_EQ(0, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_ARM_START);
	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	trigger.OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(1, receiver->GetNumTimesCalled());

	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_UP;
	trigger.OnAction(action);
	ASSERT_EQ(1, receiver->GetNumTimesCalled());

	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_UP;
	trigger.OnAction(action);
	ASSERT_EQ(1, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_OVERDUB_START);
	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	trigger.OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(2, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_OVERDUB_END);
	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_UP;
	trigger.OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(3, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_ARM_END);
	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	trigger.OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(4, receiver->GetNumTimesCalled());

	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	trigger.OnAction(action);
	ASSERT_EQ(4, receiver->GetNumTimesCalled());

	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_UP;
	trigger.OnAction(action);
	ASSERT_EQ(4, receiver->GetNumTimesCalled());
}

TEST(Trigger, OverDubReleasingDitchFirst) {
	auto receiver = std::make_shared<MockedTriggerReceiver>();
	auto trigger = MakeDefaultTrigger(receiver);
	auto action = KeyAction();

	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	trigger.OnAction(action);
	ASSERT_EQ(0, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_ARM_START);
	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	trigger.OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(1, receiver->GetNumTimesCalled());

	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_UP;
	trigger.OnAction(action);
	ASSERT_EQ(1, receiver->GetNumTimesCalled());

	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_UP;
	trigger.OnAction(action);
	ASSERT_EQ(1, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_OVERDUB_START);
	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	trigger.OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(2, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_OVERDUB_END);
	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_UP;
	trigger.OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(3, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_ARM_END);
	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	trigger.OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(4, receiver->GetNumTimesCalled());

	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	trigger.OnAction(action);
	ASSERT_EQ(4, receiver->GetNumTimesCalled());

	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_UP;
	trigger.OnAction(action);
	ASSERT_EQ(4, receiver->GetNumTimesCalled());
}

TEST(Trigger, OverDubNotReleasingActivateBeforeDub) {
	auto receiver = std::make_shared<MockedTriggerReceiver>();
	auto trigger = MakeDefaultTrigger(receiver);
	auto action = KeyAction();

	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	trigger.OnAction(action);
	ASSERT_EQ(0, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_ARM_START);
	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	trigger.OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(1, receiver->GetNumTimesCalled());

	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_UP;
	trigger.OnAction(action);
	ASSERT_EQ(1, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_OVERDUB_START);
	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	trigger.OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(2, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_OVERDUB_END);
	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_UP;
	trigger.OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(3, receiver->GetNumTimesCalled());

	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	trigger.OnAction(action);
	ASSERT_EQ(3, receiver->GetNumTimesCalled());

	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_UP;
	trigger.OnAction(action);
	ASSERT_EQ(3, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_ARM_END);
	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	trigger.OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(4, receiver->GetNumTimesCalled());

	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_UP;
	trigger.OnAction(action);
	ASSERT_EQ(4, receiver->GetNumTimesCalled());
}
