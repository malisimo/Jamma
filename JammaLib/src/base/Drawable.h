#pragma once
#include "DrawContext.h"

class Drawable
{
public:
	Drawable();
	~Drawable();

public:
	virtual void Draw(DrawContext& ctx);
	virtual bool Destroy();
};

