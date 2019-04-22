#include "Drawable.h"

using namespace base;
using resources::TextureResource;

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
