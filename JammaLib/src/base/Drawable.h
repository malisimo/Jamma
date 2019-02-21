#pragma once

#include <memory>
#include "DrawContext.h"
#include "../resources/ResourceLib.h"
#include "../resources/TextureResource.h"

class DrawableParams
{
public:
	std::string Texture;
};

class Drawable
{
public:
	Drawable(DrawableParams params);
	~Drawable();

public:
	virtual void Draw(DrawContext& ctx);
	virtual bool InitResources(ResourceLib& resourceLib);
	virtual bool ReleaseResources();

protected:
	DrawableParams _drawParams;
	std::weak_ptr<TextureResource> _texture;
};