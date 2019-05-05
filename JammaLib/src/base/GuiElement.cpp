#include "GuiElement.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

using namespace base;
using namespace utils;
using graphics::GlDrawContext;
using graphics::ImageParams;
using resources::ResourceLib;

GuiElement::GuiElement(GuiElementParams params) :
	Drawable(params),
	Moveable(params),
	Sizeable(params),
	_guiParams(params),
	_state(STATE_NORMAL),
	_texture(ImageParams(DrawableParams{ params.Texture }, SizeableParams{ params.Size,params.MinSize }, "texture")),
	_overTexture(ImageParams(DrawableParams{ params.OverTexture }, SizeableParams{ params.Size,params.MinSize }, "texture")),
	_downTexture(ImageParams(DrawableParams{ params.DownTexture }, SizeableParams{ params.Size,params.MinSize }, "texture")),
	_outTexture(ImageParams(DrawableParams{ params.OutTexture }, SizeableParams{ params.Size,params.MinSize }, "texture"))
{
}

void GuiElement::SetSize(Size2d size)
{
	Sizeable::SetSize(size);

	_texture.SetSize(_sizeParams.Size);
	_overTexture.SetSize(_sizeParams.Size);
	_downTexture.SetSize(_sizeParams.Size);
	_outTexture.SetSize(_sizeParams.Size);
}

void GuiElement::Draw(DrawContext& ctx)
{
	auto &glCtx = dynamic_cast<GlDrawContext&>(ctx);
	glCtx.PushMvp(glm::translate(glm::mat4(1.0), glm::vec3(_moveParams.Position.X, _moveParams.Position.Y, 0.f)));

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
		child->Draw(ctx);

	glCtx.PopMvp();
}

bool GuiElement::HitTest(Position2d pos)
{
	auto localPos = ToLocal(pos);

	for (auto& child : _children)
	{
		if (child->HitTest(localPos))
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

	for (auto& child : _children)
		child->InitResources(resourceLib);

	return false;
}

bool GuiElement::ReleaseResources()
{
	for (auto& child : _children)
		child->ReleaseResources();

	_texture.ReleaseResources();
	_overTexture.ReleaseResources();
	_downTexture.ReleaseResources();
	_outTexture.ReleaseResources();

	return true;
}

utils::Position2d GuiElement::ToLocal(utils::Position2d pos)
{
	return pos - _moveParams.Position;
}
