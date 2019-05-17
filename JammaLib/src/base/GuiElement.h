#pragma once

#include <tuple>
#include <vector>
#include "CommonTypes.h"
#include "Drawable.h"
#include "Sizeable.h"
#include "Moveable.h"
#include "../graphics/GlDrawContext.h"
#include "../graphics/Image.h"
#include "ActionReceiver.h"

namespace base
{
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
			DownTexture(downTexture),
			OutTexture(outTexture),
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

	public:
		enum GuiElementState
		{
			STATE_NORMAL,
			STATE_OVER,
			STATE_DOWN,
			STATE_OUT
		};

	public:
		virtual void SetSize(utils::Size2d size) override;
		virtual void Draw(DrawContext& ctx) override;
		virtual bool HitTest(utils::Position2d pos);

		virtual actions::ActionResult OnAction(actions::KeyAction action) override;
		virtual actions::ActionResult OnAction(actions::TouchAction action) override;
		virtual actions::ActionResult OnAction(actions::TouchMoveAction action) override;

	protected:
		virtual bool _InitResources(resources::ResourceLib& resourceLib) override;
		virtual bool _ReleaseResources() override;

		utils::Position2d ToLocal(utils::Position2d pos);

	protected:
		GuiElementParams _guiParams;
		GuiElementState _state;
		graphics::Image _texture;
		graphics::Image _overTexture;
		graphics::Image _downTexture;
		graphics::Image _outTexture;
		std::vector<std::shared_ptr<GuiElement>> _children;
	};
}
