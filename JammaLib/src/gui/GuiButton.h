#pragma once

#include <memory>
#include "GuiElement.h"
#include "Touchable.h"
#include "ActionReceiver.h"

namespace gui
{
	class GuiButtonParams :
		public base::GuiElementParams,
		public base::TouchableParams
	{
	public:
		GuiButtonParams(base::GuiElementParams guiParams,
			base::TouchableParams touchParams,
			std::weak_ptr<base::ActionReceiver> receiver) :
			base::GuiElementParams(guiParams),
			base::TouchableParams(touchParams),
			Receiver(receiver)
		{}

	public:
		std::weak_ptr<base::ActionReceiver> Receiver;
	};

	class GuiButton :
		public base::GuiElement,
		public base::Touchable
	{
	public:
		GuiButton(GuiButtonParams guiParams);

	public:
		void SetReceiver(std::weak_ptr<base::ActionReceiver> receiver);

		virtual void Draw(base::DrawContext& ctx) override;
		virtual void OnTouchBegin(TouchType touchType, int num) override;
		virtual void OnTouchEnd(TouchType touchType, int num) override;
		virtual void OnDrag(TouchType touchType, int num) override;

	private:
		GuiButtonParams _buttonParams;
	};
}
