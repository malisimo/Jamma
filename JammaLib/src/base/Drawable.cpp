#include "Drawable.h"

Drawable::Drawable(DrawableParams params) :
	ResourceUser(),
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
