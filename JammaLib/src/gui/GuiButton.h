#pragma once

#include <memory>
#include "GuiElement.h"
#include "Touchable.h"
#include "ActionReceiver.h"

class GuiButtonParams :
	public GuiElementParams,
	public TouchableParams
{
public:
	GuiButtonParams(GuiElementParams guiParams,
		TouchableParams touchParams,
		std::weak_ptr<ActionReceiver> receiver) :
		GuiElementParams(guiParams),
		TouchableParams(touchParams),
		Receiver(receiver)
	{}

public:
	std::weak_ptr<ActionReceiver> Receiver;
};

class GuiButton :
	public GuiElement,
	public Touchable
{
public:
	GuiButton(GuiButtonParams guiParams);
	~GuiButton();

public:
	void SetReceiver(std::weak_ptr<ActionReceiver> receiver);

	virtual void Draw(DrawContext& ctx) override;
	virtual void OnTouchBegin(TouchType touchType, int num) override;
	virtual void OnTouchEnd(TouchType touchType, int num) override;
	virtual void OnDrag(TouchType touchType, int num) override;

private:
	GuiButtonParams _buttonParams;
};

