#pragma once

#include <algorithm>
#include <functional>
#include "../utils/CommonTypes.h"
#include "GuiElement.h"
#include "ActionSender.h"
#include "ActionUndo.h"
#include "../resources/TextureResource.h"

namespace gui
{
	class GuiSliderParams;

	class GuiSlider :
		public base::GuiElement
	{
	public:
		GuiSlider(GuiSliderParams guiParams);

	public:
		double Value() const;
		void SetValue(double value);
		void SetValue(double value, bool bypassUpdate);

		virtual bool HitTest(utils::Position2d pos) override;
		virtual void Draw(base::DrawContext& ctx) override;
		virtual actions::ActionResult OnAction(actions::TouchAction action) override;
		virtual actions::ActionResult OnAction(actions::TouchMoveAction action) override;
		virtual bool Undo(std::shared_ptr<base::ActionUndo> undo) override;
		virtual bool Redo(std::shared_ptr<base::ActionUndo> undo) override;

	protected:
		virtual bool _InitResources(resources::ResourceLib& resourceLib) override;
		virtual bool _ReleaseResources() override;

		void OnValueChange(bool bypassUpdate);
		std::function<double(utils::Position2d,utils::Position2d,double)> _calcValueOffsetFun;
		std::function<utils::Position2d(double)> _calcDragPosFun;

	private:
		bool _isDragging;
		utils::Position2d _initClickPos;
		utils::Position2d _initDragPos;
		base::GuiElement _dragElement;
		double _valueOffset;
		double _initValue;
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
		double InitValue;
		std::string DragTexture;
		std::string DragOverTexture;
		std::string DragDownTexture;
		std::string DragOutTexture;
		utils::Position2d DragControlOffset;
		utils::Size2d DragControlSize;
	};
}
