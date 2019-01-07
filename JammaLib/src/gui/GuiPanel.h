#pragma once

#include "Drawable.h"

class GuiPanel : public Drawable
{
public:
	GuiPanel();
	~GuiPanel();

public:
	void Draw(DrawContext& ctx) override;
};

