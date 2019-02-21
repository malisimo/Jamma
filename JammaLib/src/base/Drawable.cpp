#include "Drawable.h"

Drawable::Drawable(DrawableParams params) :
	_drawParams(params),
	_texture(std::weak_ptr<TextureResource>())
{
}

Drawable::~Drawable()
{
}

void Drawable::Draw(DrawContext& ctx)
{
}

bool Drawable::InitResources(ResourceLib& resourceLib)
{
	return true;
}

bool Drawable::ReleaseResources()
{
	return true;
}
