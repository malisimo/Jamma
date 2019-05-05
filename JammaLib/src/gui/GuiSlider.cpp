#include "GuiSlider.h"
#include "glm/ext.hpp"

using namespace gui;
using namespace utils;
using namespace base;
using namespace actions;
using graphics::GlDrawContext;
using resources::ResourceLib;

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
		SizeableParams{ params.DragControlSize, params.DragControlSize },
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

	_dragElement.SetPosition(_calcDragPosFun(0.0f));
}

GuiSlider::~GuiSlider()
{
}

double GuiSlider::Value() const
{
	return _calcValueFun(_dragElement.Position());
}

bool GuiSlider::HitTest(Position2d pos)
{
	auto localPos = pos - _moveParams.Position;

	for (auto& child : _children)
	{
		if (child->HitTest(localPos))
			return true;
	}

	if (localPos.X > _dragElement.Position().X && localPos.X < _dragElement.Position().X + (int)_dragElement.GetSize().Width)
	{
		auto res = (localPos.Y > _dragElement.Position().Y && localPos.Y < _dragElement.Position().X + (int)_dragElement.GetSize().Height);

		return res;
	}

	return false;
}

void GuiSlider::Draw(DrawContext & ctx)
{
	GuiElement::Draw(ctx);

	auto &glCtx = dynamic_cast<GlDrawContext&>(ctx);
	glCtx.PushMvp(glm::translate(glm::mat4(1.0), glm::vec3(_moveParams.Position.X, _moveParams.Position.Y, 0.f)));
	_dragElement.Draw(ctx);
	glCtx.PopMvp();
}

void GuiSlider::OnAction(TouchAction action)
{
	auto localPos = ToLocal(action.Position);

	if (_isDragging)
	{
		if (TouchAction::TOUCH_UP == action.State)
		{
			_isDragging = false;

			if (_dragElement.HitTest(localPos))
			{
				std::cout << "Slider UP" << std::endl;
				//_receiver.OnAction();
			}
		}
	}
	else
	{
		if (TouchAction::TOUCH_DOWN == action.State)
		{
			if (_dragElement.HitTest(localPos))
			{
				std::cout << "Slider DOWN" << std::endl;
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

	auto dPos = action.Position - _initClickPos;
	auto dragPos = _initDragPos + dPos;	
	auto newValue = _calcValueFun(dragPos);

	_dragElement.SetPosition(_calcDragPosFun(newValue));
}

bool GuiSlider::InitResources(ResourceLib& resourceLib)
{
	_dragElement.InitResources(resourceLib);
	return GuiElement::InitResources(resourceLib);
}

bool GuiSlider::ReleaseResources()
{
	_dragElement.ReleaseResources();
	return GuiElement::ReleaseResources();
}
