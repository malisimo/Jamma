#include "WindowAction.h"
#include "Sizeable.h"

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
