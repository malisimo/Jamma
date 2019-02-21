#pragma once

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include <tuple>
#include <vector>
#include "CommonTypes.h"
#include "Drawable.h"
#include "Sizeable.h"
#include "Moveable.h"
#include "../graphics/GlDrawContext.h"
#include "../graphics/Image.h"
#include "ActionReceiver.h"

class GuiElementParams :
	public DrawableParams,
	public MoveableParams,
	public SizeableParams
{
public:
	GuiElementParams(DrawableParams drawParams,
		MoveableParams moveParams,
		SizeableParams sizeParams,
		std::string overTexture,
		std::string downTexture,
		std::string outTexture,
		std::vector<GuiElementParams> childParams) :
		DrawableParams(drawParams),
		MoveableParams(moveParams),
		SizeableParams(sizeParams),
		OverTexture(overTexture),
		DownTexture(overTexture),
		OutTexture(overTexture),
		ChildParams(childParams)
	{
	}

public:
	std::string OverTexture;
	std::string DownTexture;
	std::string OutTexture;
	std::vector<GuiElementParams> ChildParams;
};

class GuiElement : public Drawable, public Sizeable, public Moveable, public ActionReceiver
{
public:
	GuiElement(GuiElementParams params);
	~GuiElement();

public:
	enum GuiElementState
	{
		STATE_NORMAL,
		STATE_OVER,
		STATE_DOWN,
		STATE_OUT
	};

public:
	virtual void Draw(DrawContext& ctx) override;
	virtual bool HitTest(Position2d pos);
	virtual bool InitResources(ResourceLib& resourceLib) override;
	virtual bool ReleaseResources() override;

protected:
	virtual void UpdateChildren();

protected:
	GuiElementParams _guiParams;
	GuiElementState _state;
	Image _texture;
	Image _overTexture;
	Image _downTexture;
	Image _outTexture;
	std::vector<GuiElement> _children;
};