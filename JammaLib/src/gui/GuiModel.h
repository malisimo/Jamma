#pragma once

#include "GuiElement.h"

class GuiModelParams : public GuiElementParams
{
};

class GuiModel :
	public GuiElement
{
public:
	GuiModel(GuiModelParams params);
	~GuiModel();

public:
	void Draw(DrawContext& ctx) override;

protected:
	GuiModelParams _modelParams;
};
