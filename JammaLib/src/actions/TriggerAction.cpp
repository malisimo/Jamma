#include "TriggerAction.h"

using namespace actions;

TriggerAction::TriggerAction() :
	ActionType(TriggerActionType::TRIGGER_REC_START),
	TargetId(0),
	OverbudTargetId(0),
	SampleCount(0),
	InputChannels({})
{
}

TriggerAction::~TriggerAction()
{
}
