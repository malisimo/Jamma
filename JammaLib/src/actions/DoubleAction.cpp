#include "DoubleAction.h"

using namespace actions;
using base::ActionSender;
using base::ActionReceiver;

DoubleAction::DoubleAction(double value) :
	_value(value),
	Action()
{
}

DoubleAction::~DoubleAction()
{
}

double DoubleAction::Value() const
{
	return _value;
}

DoubleActionUndo::DoubleActionUndo(double value,
	std::weak_ptr<ActionSender> sender) :
	_value(value),
	ActionUndo(sender)
{
}

DoubleActionUndo::~DoubleActionUndo()
{
}

double DoubleActionUndo::Value() const
{
	return _value;
}
