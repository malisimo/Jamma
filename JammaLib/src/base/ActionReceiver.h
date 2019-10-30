#pragma once

#include <memory>
#include <optional>
#include "Action.h"
#include "Actionable.h"
#include "../io/UserConfig.h"
#include "../actions/WindowAction.h"
#include "../actions/TouchAction.h"
#include "../actions/TouchMoveAction.h"
#include "../actions/KeyAction.h"
#include "../actions/DoubleAction.h"
#include "../actions/TriggerAction.h"
#include "../actions/JobAction.h"
#include "../actions/ActionResult.h"

namespace base
{
	class ActionReceiver :
		public virtual Actionable
	{
	public:
		virtual ActionDirection Direction() const override { return ACTIONDIR_RECEIVE; }
		virtual actions::ActionResult OnAction(actions::WindowAction action, std::optional<io::UserConfig> cfg)		{ return { false, actions::ACTIONRESULT_DEFAULT }; };
		virtual actions::ActionResult OnAction(actions::TouchAction action, std::optional<io::UserConfig> cfg)		{ return { false, actions::ACTIONRESULT_DEFAULT }; };
		virtual actions::ActionResult OnAction(actions::TouchMoveAction action, std::optional<io::UserConfig> cfg)	{ return { false, actions::ACTIONRESULT_DEFAULT }; };
		virtual actions::ActionResult OnAction(actions::KeyAction action, std::optional<io::UserConfig> cfg)		{ return { false, actions::ACTIONRESULT_DEFAULT }; };
		virtual actions::ActionResult OnAction(actions::DoubleAction action, std::optional<io::UserConfig> cfg)		{ return { false, actions::ACTIONRESULT_DEFAULT }; };
		virtual actions::ActionResult OnAction(actions::TriggerAction action, std::optional<io::UserConfig> cfg)	{ return { false, actions::ACTIONRESULT_DEFAULT }; };
		virtual actions::ActionResult OnAction(actions::JobAction action, std::optional<io::UserConfig> cfg)		{ return { false, actions::ACTIONRESULT_DEFAULT }; };

		std::shared_ptr<ActionReceiver> shared_from_this()
		{
			return std::dynamic_pointer_cast<ActionReceiver>(
				Sharable::shared_from_this());
		}

	protected:
		virtual void InitReceivers() {};
	};
}
