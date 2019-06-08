#pragma once

#include <memory>
#include "../base/ActionUndo.h"

namespace base { class GuiElement; };

namespace actions
{
	struct ActionResult
	{
		bool IsEaten;
		std::shared_ptr<base::ActionUndo> Undo;
		std::weak_ptr<base::GuiElement> ActiveElement;
	};
};
