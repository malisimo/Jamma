#include "WindowAction.h"

using namespace actions;

WindowAction::WindowAction()
{
}

WindowAction::~WindowAction()
{
}

/*void WindowAction::Apply(ActionReceiver& receiver)
{
	switch (WindowActionType)
	{
	case WindowAction::SIZE:
		auto& sizeable = dynamic_cast<Sizeable&>(receiver);
		sizeable.SetSize(Size);
	}
}*/
