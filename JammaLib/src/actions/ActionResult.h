#pragma once

#include <memory>
#include "../base/ActionUndo.h"

namespace base { class GuiElement; };

namespace actions
{
	enum ActionResultType
	{
		ACTIONRESULT_DEFAULT = 0,
		ACTIONRESULT_ACTIVEELEMENT = 1,
		ACTIONRESULT_MASTERLOOP = 2
	};

	struct ActionResult
	{
		bool IsEaten;
		ActionResultType ResultType;
		std::shared_ptr<base::ActionUndo> Undo;
		std::weak_ptr<base::GuiElement> ActiveElement;
		std::shared_ptr<base::GuiElement> MasterLoop;
	};
};
