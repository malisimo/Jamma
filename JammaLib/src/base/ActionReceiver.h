#pragma once

#include <memory>
#include "Action.h"
#include "../actions/WindowAction.h"
#include "../actions/TouchAction.h"
#include "../actions/TouchMoveAction.h"
#include "../actions/KeyAction.h"

namespace base
{
	class ActionReceiver
	{
	public:
		ActionReceiver();
		~ActionReceiver();

	public:
		virtual void OnAction(actions::WindowAction action);
		virtual void OnAction(actions::TouchAction action);
		virtual void OnAction(actions::TouchMoveAction action);
		virtual void OnAction(actions::KeyAction action);
	};
}
