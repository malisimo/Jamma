#pragma once

#include <memory>
#include "Action.h"
#include "Actionable.h"
#include "../actions/WindowAction.h"
#include "../actions/TouchAction.h"
#include "../actions/TouchMoveAction.h"
#include "../actions/KeyAction.h"
#include "../actions/DoubleAction.h"
#include "../actions/ActionResult.h"

namespace base
{
	class ActionReceiver :
		public virtual Actionable
	{
	public:
		virtual actions::ActionResult OnAction(actions::WindowAction action) { return { false, nullptr }; };
		virtual actions::ActionResult OnAction(actions::TouchAction action) { return { false, nullptr }; };
		virtual actions::ActionResult OnAction(actions::TouchMoveAction action) { return { false, nullptr }; };
		virtual actions::ActionResult OnAction(actions::KeyAction action) { return { false, nullptr }; };
		virtual actions::ActionResult OnAction(actions::DoubleAction action) { return { false, nullptr }; };

		virtual void InitReceivers() {};

		std::shared_ptr<ActionReceiver> shared_from_this()
		{
			return std::dynamic_pointer_cast<ActionReceiver>(
				Actionable::shared_from_this());
		}
	};
}
