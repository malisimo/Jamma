
#include "gtest/gtest.h"
#include "resources/ResourceLib.h"
#include "engine/Trigger.h"

using base::ActionSender;
using base::ActionReceiver;
using engine::Trigger;
using engine::TriggerParams;
using engine::Timer;
using actions::TriggerAction;
using actions::KeyAction;

const unsigned int ActivateChar = 49;
const unsigned int DitchChar = 50;
const unsigned int ActivateMidi = 211;
const unsigned int DitchMidi = 212;

Time GetTime(unsigned int seconds)
{
	auto t = std::chrono::steady_clock::now();
	return t + std::chrono::seconds(seconds);
}

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

std::unique_ptr<Trigger> MakeDefaultTrigger(std::shared_ptr<MockedTriggerReceiver> receiver)
{
	auto activateBind = engine::DualBinding(0);
	activateBind.SetDown(engine::TriggerBinding(engine::TRIGGER_KEY, ActivateChar, 1), true);

	auto ditchBind = engine::DualBinding(0);
	ditchBind.SetRelease(engine::TriggerBinding(engine::TRIGGER_KEY, DitchChar, 0), true);

	TriggerParams trigParams;
	trigParams.Activate = { activateBind };
	TriggerParams ditchParams;
	trigParams.Ditch = { ditchBind };
	auto trigger = std::make_unique<Trigger>(trigParams);
	trigger->SetReceiver(receiver);

	return std::move(trigger);
}

TEST(Trigger, DitchesLoop) {
	auto receiver = std::make_shared<MockedTriggerReceiver>();
	auto trigger = MakeDefaultTrigger(receiver);
	auto action = KeyAction();;
	actions::ActionResult actionRes;

	receiver->SetExpected(TriggerAction::TRIGGER_REC_START);
	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	action.SetActionTime(GetTime(0));
	actionRes = trigger->OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());

	receiver->SetExpected(TriggerAction::TRIGGER_DITCH);
	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_UP;
	action.SetActionTime(GetTime(1));
	actionRes = trigger->OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
}

TEST(Trigger, RecordsTwoLoops) {
	auto receiver = std::make_shared<MockedTriggerReceiver>();
	auto trigger = MakeDefaultTrigger(receiver);
	auto action = KeyAction();
	actions::ActionResult actionRes;

	receiver->SetExpected(TriggerAction::TRIGGER_REC_START);
	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	action.SetActionTime(GetTime(0));
	actionRes = trigger->OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(1, receiver->GetNumTimesCalled());

	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_UP;
	action.SetActionTime(GetTime(1));
	actionRes = trigger->OnAction(action);
	ASSERT_EQ(1, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_REC_END);
	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	action.SetActionTime(GetTime(2));
	actionRes = trigger->OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(2, receiver->GetNumTimesCalled());

	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_UP;
	action.SetActionTime(GetTime(3));
	actionRes = trigger->OnAction(action);
	ASSERT_EQ(2, receiver->GetNumTimesCalled());
}

TEST(Trigger, NoReleaseSkipsAction) {
	auto receiver = std::make_shared<MockedTriggerReceiver>();
	auto trigger = MakeDefaultTrigger(receiver);
	auto action = KeyAction();
	actions::ActionResult actionRes;

	receiver->SetExpected(TriggerAction::TRIGGER_REC_START);
	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	action.SetActionTime(GetTime(0));
	actionRes = trigger->OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());

	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	action.SetActionTime(GetTime(1));
	actionRes = trigger->OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(1, receiver->GetNumTimesCalled());
}

TEST(Trigger, OverDubReleasingActivateFirst) {
	auto receiver = std::make_shared<MockedTriggerReceiver>();
	auto trigger = MakeDefaultTrigger(receiver);
	auto action = KeyAction();
	actions::ActionResult actionRes;

	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	action.SetActionTime(GetTime(0));
	actionRes = trigger->OnAction(action);
	ASSERT_EQ(0, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_OVERDUB_START);
	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	action.SetActionTime(GetTime(1));
	actionRes = trigger->OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(1, receiver->GetNumTimesCalled());

	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_UP;
	action.SetActionTime(GetTime(2));
	actionRes = trigger->OnAction(action);
	ASSERT_EQ(1, receiver->GetNumTimesCalled());

	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_UP;
	action.SetActionTime(GetTime(3));
	actionRes = trigger->OnAction(action);
	ASSERT_EQ(1, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_PUNCHIN_START);
	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	action.SetActionTime(GetTime(4));
	actionRes = trigger->OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(2, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_PUNCHIN_END);
	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_UP;
	action.SetActionTime(GetTime(5));
	actionRes = trigger->OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(3, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_OVERDUB_END);
	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	action.SetActionTime(GetTime(6));
	actionRes = trigger->OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(4, receiver->GetNumTimesCalled());

	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	action.SetActionTime(GetTime(7));
	actionRes = trigger->OnAction(action);
	ASSERT_EQ(4, receiver->GetNumTimesCalled());

	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_UP;
	action.SetActionTime(GetTime(8));
	actionRes = trigger->OnAction(action);
	ASSERT_EQ(4, receiver->GetNumTimesCalled());
}

TEST(Trigger, OverDubReleasingDitchFirst) {
	auto receiver = std::make_shared<MockedTriggerReceiver>();
	auto trigger = MakeDefaultTrigger(receiver);
	auto action = KeyAction();
	actions::ActionResult actionRes;

	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	action.SetActionTime(GetTime(0));
	actionRes = trigger->OnAction(action);
	ASSERT_EQ(0, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_OVERDUB_START);
	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	action.SetActionTime(GetTime(1));
	actionRes = trigger->OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(1, receiver->GetNumTimesCalled());

	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_UP;
	action.SetActionTime(GetTime(2));
	actionRes = trigger->OnAction(action);
	ASSERT_EQ(1, receiver->GetNumTimesCalled());

	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_UP;
	action.SetActionTime(GetTime(3));
	actionRes = trigger->OnAction(action);
	ASSERT_EQ(1, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_PUNCHIN_START);
	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	action.SetActionTime(GetTime(4));
	actionRes = trigger->OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(2, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_PUNCHIN_END);
	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_UP;
	action.SetActionTime(GetTime(5));
	actionRes = trigger->OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(3, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_OVERDUB_END);
	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	action.SetActionTime(GetTime(6));
	actionRes = trigger->OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(4, receiver->GetNumTimesCalled());

	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	action.SetActionTime(GetTime(7));
	actionRes = trigger->OnAction(action);
	ASSERT_EQ(4, receiver->GetNumTimesCalled());

	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_UP;
	action.SetActionTime(GetTime(8));
	actionRes = trigger->OnAction(action);
	ASSERT_EQ(4, receiver->GetNumTimesCalled());
}

TEST(Trigger, OverDubNotReleasingActivateBeforeDub) {
	auto receiver = std::make_shared<MockedTriggerReceiver>();
	auto trigger = MakeDefaultTrigger(receiver);
	auto action = KeyAction();
	actions::ActionResult actionRes;

	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	action.SetActionTime(GetTime(0));
	actionRes = trigger->OnAction(action);
	ASSERT_EQ(0, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_OVERDUB_START);
	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	action.SetActionTime(GetTime(1));
	actionRes = trigger->OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(1, receiver->GetNumTimesCalled());

	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_UP;
	action.SetActionTime(GetTime(2));
	actionRes = trigger->OnAction(action);
	ASSERT_EQ(1, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_PUNCHIN_START);
	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	action.SetActionTime(GetTime(3));
	actionRes = trigger->OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(2, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_PUNCHIN_END);
	action.KeyChar = DitchChar;
	action.KeyActionType = KeyAction::KEY_UP;
	action.SetActionTime(GetTime(4));
	actionRes = trigger->OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(3, receiver->GetNumTimesCalled());

	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	action.SetActionTime(GetTime(5));
	actionRes = trigger->OnAction(action);
	ASSERT_EQ(3, receiver->GetNumTimesCalled());

	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_UP;
	action.SetActionTime(GetTime(6));
	actionRes = trigger->OnAction(action);
	ASSERT_EQ(3, receiver->GetNumTimesCalled());

	receiver->SetExpected(TriggerAction::TRIGGER_OVERDUB_END);
	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_DOWN;
	action.SetActionTime(GetTime(7));
	actionRes = trigger->OnAction(action);
	ASSERT_TRUE(receiver->GetLastMatched());
	ASSERT_EQ(4, receiver->GetNumTimesCalled());

	action.KeyChar = ActivateChar;
	action.KeyActionType = KeyAction::KEY_UP;
	action.SetActionTime(GetTime(8));
	actionRes = trigger->OnAction(action);
	ASSERT_EQ(4, receiver->GetNumTimesCalled());
}
