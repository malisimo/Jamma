#pragma once

#include <algorithm>
#include <functional>
#include "../utils/CommonTypes.h"
#include "../base/GuiElement.h"
#include "../resources/TextureResource.h"

namespace gui
{
	class GuiSliderParams;

	class GuiSlider : public base::GuiElement
	{
	public:
		GuiSlider(GuiSliderParams guiParams);

	public:
		double Value() const;

		virtual bool HitTest(utils::Position2d pos) override;
		virtual void Draw(base::DrawContext& ctx) override;
		virtual actions::ActionResult OnAction(actions::TouchAction action) override;
		virtual actions::ActionResult OnAction(actions::TouchMoveAction action) override;
		virtual bool InitResources(resources::ResourceLib& resourceLib) override;
		virtual bool ReleaseResources() override;

	private:
		std::function<double(utils::Position2d)> _calcValueFun;
		std::function<utils::Position2d(double)> _calcDragPosFun;

	private:
		bool _isDragging;
		utils::Position2d _initClickPos;
		utils::Position2d _initDragPos;
		base::GuiElement _dragElement;
	};

	class GuiSliderParams : public base::GuiElementParams
	{
	public:
		GuiSliderParams() :
			base::GuiElementParams(DrawableParams{ "" },
				MoveableParams{ 0,0 },
				SizeableParams{ 1,1 },
				"",
				"",
				"",
				{}),
			Orientation(SLIDER_VERTICAL),
			Min(0.0),
			Max(1.0),
			DragLength(1),
			Steps(0),
			Quantised(false),
			DragTexture(""),
			DragOverTexture(""),
			DragDownTexture(""),
			DragOutTexture(""),
			DragControlOffset({ 0,0 }),
			DragControlSize({ 1,1 })
		{
		}

		GuiSliderParams(GuiElementParams params) :
			base::GuiElementParams(params),
			Orientation(SLIDER_VERTICAL),
			Min(0.0),
			Max(1.0),
			DragLength(1),
			Steps(0),
			Quantised(false),
			DragTexture(""),
			DragOverTexture(""),
			DragDownTexture(""),
			DragOutTexture(""),
			DragControlOffset({ 0,0 }),
			DragControlSize({ 1,1 })
		{
		}

		enum SliderOrientation
		{
			SLIDER_VERTICAL,
			SLIDER_HORIZONTAL
		};

	public:
		SliderOrientation Orientation;
		double Min;
		double Max;
		int DragLength;
		int Steps;
		bool Quantised;
		std::string DragTexture;
		std::string DragOverTexture;
		std::string DragDownTexture;
		std::string DragOutTexture;
		utils::Position2d DragControlOffset;
		utils::Size2d DragControlSize;
	};
}
