#include "GuiSlider.h"

GuiSlider::GuiSlider(GuiSliderParams params) :
	GuiElement(params),
	_isDragging(false),
	_initClickPos({ 0,0 }),
	_initDragPos({ 0,0 }),
	_calcValueFun(),
	_calcDragPosFun(),
	_dragElement(GuiElementParams(
		DrawableParams{ params.DragTexture },
		MoveableParams{ params.Position },
		SizeableParams{ params.Size, params.MinSize },
		params.DragOverTexture,
		params.DragDownTexture,
		params.DragOutTexture,
		{} ))
{
	_calcValueFun = [params](Position2d dragPos) {
		auto valRange = params.Max - params.Min;
		auto dragFrac = GuiSliderParams::SLIDER_VERTICAL == params.Orientation ?
			std::clamp(dragPos.Y - params.DragControlOffset.Y, 0, params.DragLength) / (double)params.DragLength :
			std::clamp(dragPos.X - params.DragControlOffset.X, 0, params.DragLength) / (double)params.DragLength;
		
		if (params.Quantised)
		{
			auto dStep = valRange / params.Steps;
			auto stepNum = (int)round(dragFrac * params.Steps);
			return params.Min + (stepNum * dStep);
		}

		return (valRange * dragFrac) + params.Min;
	};

	_calcDragPosFun = [params](double value) {
		auto valRange = params.Max - params.Min;
		auto valFrac = (value - params.Min) / valRange;

		return GuiSliderParams::SLIDER_VERTICAL == params.Orientation ?
			Position2d{ params.DragControlOffset.X, ((int)round(valFrac * params.DragLength)) + params.DragControlOffset.Y } :
			Position2d{ ((int)round(valFrac * params.DragLength)) + params.DragControlOffset.X, params.DragControlOffset.Y };
	};
}

GuiSlider::~GuiSlider()
{
}

double GuiSlider::Value() const
{
	return _calcValueFun(_dragElement.Position());
}

void GuiSlider::Draw(DrawContext & ctx)
{
	_dragElement.Draw(ctx);
	GuiElement::Draw(ctx);
}

void GuiSlider::OnAction(TouchAction action)
{
	if (_isDragging)
	{
		if (TouchAction::TOUCH_UP == action.State)
		{
			_isDragging = false;

			if (_dragElement.HitTest(action.Position))
			{
				//_receiver.OnAction();
			}
		}
	}
	else
	{
		if (TouchAction::TOUCH_DOWN == action.State)
		{
			if (_dragElement.HitTest(action.Position))
			{
				_isDragging = true;
				_initClickPos = action.Position;
				_initDragPos = _dragElement.Position();
			}
		}
	}
}

void GuiSlider::OnAction(TouchMoveAction action)
{
	if (!_isDragging)
		return;

	auto dPos = _initClickPos - action.Position;
	auto dragPos = _initDragPos + dPos;	
	auto newValue = _calcValueFun(dragPos);

	_dragElement.SetPosition(_calcDragPosFun(newValue));
}

GuiSliderParams::GuiSliderParams() :
	GuiElementParams(DrawableParams{ "" },
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
