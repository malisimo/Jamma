#pragma once

#include <memory>
#include "Action.h"
#include "../actions/WindowAction.h"
#include "../actions/TouchAction.h"
#include "../actions/TouchMoveAction.h"
#include "../actions/KeyAction.h"

class ActionReceiver
{
public:
	ActionReceiver();
	~ActionReceiver();

public:
	virtual void OnAction(WindowAction action);
	virtual void OnAction(TouchAction action);
	virtual void OnAction(TouchMoveAction action);
	virtual void OnAction(KeyAction action);
};
