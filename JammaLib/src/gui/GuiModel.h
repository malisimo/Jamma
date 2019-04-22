#pragma once

#include "GuiElement.h"

namespace gui
{
	class GuiModelParams : public base::GuiElementParams
	{
	};

	class GuiModel :
		public base::GuiElement
	{
	public:
		GuiModel(GuiModelParams params);
		~GuiModel();

	public:
		void Draw(base::DrawContext& ctx) override;

	protected:
		GuiModelParams _modelParams;
	};
}
