#pragma once
#include "Drawable.h"

class GuiTexture
{
public:
	GuiTexture();
	~GuiTexture();

public:
	void Draw(DrawContext& ctx);
};

