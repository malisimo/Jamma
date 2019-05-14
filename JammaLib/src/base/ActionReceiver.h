#pragma once

#include <memory>
#include "Action.h"
#include "../actions/WindowAction.h"
#include "../actions/TouchAction.h"
#include "../actions/TouchMoveAction.h"
#include "../actions/KeyAction.h"
#include "../actions/ActionResult.h"

namespace base
{
	class ActionReceiver
	{
	public:
		virtual actions::ActionResult OnAction(actions::WindowAction action) { return { false }; };
		virtual actions::ActionResult OnAction(actions::TouchAction action) { return { false }; };
		virtual actions::ActionResult OnAction(actions::TouchMoveAction action) { return { false }; };
		virtual actions::ActionResult OnAction(actions::KeyAction action) { return { false }; };
	};
}
