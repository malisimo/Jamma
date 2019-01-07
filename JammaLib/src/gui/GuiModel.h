#pragma once

#include "Drawable.h"

class GuiModel : public Drawable
{
public:
	GuiModel();
	~GuiModel();

public:
	void Draw(DrawContext& ctx) override;
};

