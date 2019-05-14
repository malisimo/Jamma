#include "GuiButton.h"

using namespace base;
using namespace gui;
using base::Touchable;

GuiButton::GuiButton(GuiButtonParams params) :
	_buttonParams(params),
	Touchable(params),
	GuiElement(params)
{
}

void GuiButton::SetReceiver(std::weak_ptr<ActionReceiver> receiver)
{
	_buttonParams.Receiver = receiver;
}

void GuiButton::Draw(DrawContext& ctx)
{
}

void GuiButton::OnTouchBegin(TouchType touchType, int num)
{

}

void GuiButton::OnTouchEnd(TouchType touchType, int num)
{
	auto receiver = _buttonParams.Receiver.lock();

	//if (receiver)
	//	receiver->OnAction();
}

void GuiButton::OnDrag(TouchType touchType, int num)
{
}
