#include "GuiElement.h"


GuiElement::GuiElement(GuiElementParams params) :
	Drawable(params),
	Moveable(params),
	Sizeable(params),
	_guiParams(params),
	_state(STATE_NORMAL),
	_texture(ImageParams(DrawableParams{ params.Texture }, SizeableParams{ params.Size,params.MinSize })),
	_overTexture(ImageParams(DrawableParams{ params.OverTexture }, SizeableParams{ params.Size,params.MinSize })),
	_downTexture(ImageParams(DrawableParams{ params.DownTexture }, SizeableParams{ params.Size,params.MinSize })),
	_outTexture(ImageParams(DrawableParams{ params.OutTexture }, SizeableParams{ params.Size,params.MinSize }))
{
}

GuiElement::~GuiElement()
{
}

void GuiElement::UpdateChildren()
{
}

void GuiElement::Draw(DrawContext& ctx)
{
	switch (_state)
	{
	case STATE_OVER:
		_overTexture.Draw(ctx);
		break;
	case STATE_DOWN:
		_downTexture.Draw(ctx);
		break;
	case STATE_OUT:
		_outTexture.Draw(ctx);
		break;
	default:
		_texture.Draw(ctx);
		break;
	}

	for (auto& child : _children)
		child.Draw(ctx);
}

bool GuiElement::HitTest(Position2d pos)
{
	auto localPos = pos - _moveParams.Position;

	for (auto& child : _children)
	{
		if (child.HitTest(localPos))
			return true;
	}

	if (localPos.X > 0 && localPos.X < (int)_sizeParams.Size.Width)
	{
		return (localPos.Y > 0 && localPos.Y < (int)_sizeParams.Size.Height);
	}

	return false;
}

bool GuiElement::InitResources(ResourceLib& resourceLib)
{
	_texture.InitResources(resourceLib);
	_overTexture.InitResources(resourceLib);
	_downTexture.InitResources(resourceLib);
	_outTexture.InitResources(resourceLib);

	return false;
}

bool GuiElement::ReleaseResources()
{
	_texture.ReleaseResources();
	_overTexture.ReleaseResources();
	_downTexture.ReleaseResources();
	_outTexture.ReleaseResources();

	return true;
}
