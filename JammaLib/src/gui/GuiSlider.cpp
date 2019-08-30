#include "GuiSlider.h"
#include "CommonTypes.h"
#include "glm/ext.hpp"

using namespace gui;
using namespace utils;
using namespace base;
using namespace actions;
using graphics::GlDrawContext;
using resources::ResourceLib;

GuiSlider::GuiSlider(GuiSliderParams params) :
	GuiElement(params),
	_sliderParams(params),
	_isDragging(false),
	_initClickPos({ 0,0 }),
	_initDragPos({ 0,0 }),
	_initValue(params.InitValue),
	_valueOffset(0.0),
	_dragElement(GuiElementParams(
		DrawableParams{ params.DragTexture },
		MoveableParams(params.Position, params.ModelPosition, params.ModelScale),
		SizeableParams{ params.DragControlSize, params.DragControlSize },
		params.DragOverTexture,
		params.DragDownTexture,
		params.DragOutTexture,
		{} ))
{
	SetValue(params.InitValue);
	_initDragPos = _dragElement.Position();
}

double GuiSlider::Value() const
{
	return _initValue + _valueOffset;
}

void GuiSlider::SetValue(double value)
{
	SetValue(value, false);
}

void GuiSlider::SetValue(double value, bool bypassUpdates)
{
	if (_isDragging)
		return;

	_initValue = value;
	_valueOffset = 0.0;

	OnValueChange(bypassUpdates);
}

void GuiSlider::SetDragParams(utils::Position2d dragOffset,
	utils::Size2d dragSize,
	utils::Size2d dragGap)
{
	_sliderParams.DragControlOffset = dragOffset;
	_sliderParams.DragControlSize = dragSize;
	_sliderParams.DragGap = dragGap;

	OnValueChange(true);
}

void GuiSlider::SetSize(Size2d size)
{
	GuiElement::SetSize(size);

	OnValueChange(true);

	_texture.SetSize(_sizeParams.Size);
	_overTexture.SetSize(_sizeParams.Size);
	_downTexture.SetSize(_sizeParams.Size);
	_outTexture.SetSize(_sizeParams.Size);
}

bool GuiSlider::HitTest(Position2d localPos)
{
	for (auto& child : _children)
	{
		if (child->HitTest(localPos))
			return true;
	}

	auto left = _dragElement.Position().X;
	auto right = _dragElement.Position().X + (int)_dragElement.GetSize().Width;

	if (localPos.X > left && localPos.X < right)
	{
		auto bottom = _dragElement.Position().Y;
		auto top = _dragElement.Position().X + (int)_dragElement.GetSize().Height;

		if (localPos.Y > bottom && localPos.Y < top)
			return true;
	}

	return false;
}

void GuiSlider::Draw(DrawContext & ctx)
{
	GuiElement::Draw(ctx);

	auto &glCtx = dynamic_cast<GlDrawContext&>(ctx);
	auto pos = Position();
	glCtx.PushModelView(glm::translate(glm::mat4(1.0), glm::vec3(pos.X, pos.Y, 0.f)));
	_dragElement.Draw(ctx);
	glCtx.PopModelView();
}

ActionResult GuiSlider::OnAction(TouchAction action)
{
	auto res = GuiElement::OnAction(action);

	//if (res.IsEaten)
	//	return res;

	if (_isDragging)
	{
		if (TouchAction::TOUCH_UP == action.State)
		{
			_isDragging = false;
			auto oldValue = _initValue;

			std::cout << "Slider UP" << std::endl;

			_initValue = oldValue + _valueOffset;
			_valueOffset = 0.0;
			std::cout << "New InitValue: " << _initValue << ", ValueOffset: " << _valueOffset << " = " << (_initValue + _valueOffset) << std::endl;

			ActionResult res;
			res.IsEaten = true;
			res.ResultType = ACTIONRESULT_DEFAULT;
			res.Undo = std::make_shared<DoubleActionUndo>(oldValue, GuiElement::shared_from_this());

			return res;
		}
	}
	else
	{
		if (TouchAction::TOUCH_DOWN == action.State)
		{
			if (_dragElement.HitTest(_dragElement.ParentToLocal(action.Position)))
			{
				std::cout << "Slider DOWN" << std::endl;
				_isDragging = true;
				_initClickPos = action.Position;
				_initDragPos = _dragElement.Position();
				_valueOffset = 0.0;

				ActionResult res;
				res.IsEaten = true;
				res.ResultType = ACTIONRESULT_ACTIVEELEMENT;
				res.ActiveElement = GuiElement::shared_from_this();

				return res;
			}
		}
	}

	return GuiElement::OnAction(action);
}

ActionResult GuiSlider::OnAction(TouchMoveAction action)
{
	auto res = GuiElement::OnAction(action);

	if (res.IsEaten)
		return res;

	res.IsEaten = false;
	res.ResultType = ACTIONRESULT_DEFAULT;

	if (!_isDragging)
		return res;

	auto dPos = action.Position - _initClickPos;
	auto dragPos = _initDragPos + dPos;

	_valueOffset = CalcValueOffset(_sliderParams, dragPos, _initDragPos, _initValue);
	std::cout << "InitValue: " << _initValue << ", ValueOffset: " << _valueOffset << " = " << (_initValue + _valueOffset) << std::endl;

	OnValueChange(false);

	res.IsEaten = true;
	return res;
}

bool GuiSlider::Undo(std::shared_ptr<ActionUndo> undo)
{
	if (_isDragging)
		return false;

	auto doubleUndo = std::dynamic_pointer_cast<DoubleActionUndo>(undo);

	if (doubleUndo)
	{
		SetValue(doubleUndo->Value());
		return true;
	}

	return false;
}

bool GuiSlider::Redo(std::shared_ptr<ActionUndo> undo)
{
	if (_isDragging)
		return false;

	auto doubleUndo = std::dynamic_pointer_cast<DoubleActionUndo>(undo);

	if (doubleUndo)
	{
		SetValue(doubleUndo->Value());
		return true;
	}

	return false;
}

bool GuiSlider::_InitResources(ResourceLib& resourceLib)
{
	_dragElement.InitResources(resourceLib);
	return GuiElement::_InitResources(resourceLib);
}

bool GuiSlider::_ReleaseResources()
{
	_dragElement.ReleaseResources();
	return GuiElement::_ReleaseResources();
}

void GuiSlider::OnValueChange(bool bypassUpdates)
{
	auto value = _initValue + _valueOffset;
	_dragElement.SetPosition(CalcDragPos(_sliderParams, value));

	if (_receiver && !bypassUpdates)
		_receiver->OnAction(DoubleAction(value));
}

double GuiSlider::CalcValueOffset(GuiSliderParams params,
	Position2d dragPos,
	Position2d initDragPos,
	double initValue)
{
	auto valRange = params.Max - params.Min;
	double dragFrac = 0.0;

	auto dragLength = CalcDragLength(params);

	if (dragLength > 0)
		dragFrac = GuiSliderParams::SLIDER_VERTICAL == params.Orientation ?
		std::clamp(dragPos.Y - params.DragControlOffset.Y, 0, (int)dragLength) / (double)dragLength :
		std::clamp(dragPos.X - params.DragControlOffset.X, 0, (int)dragLength) / (double)dragLength;

	if (params.Steps > 0)
	{
		auto dStep = valRange / (double)params.Steps;
		auto stepNum = (int)round(dragFrac * params.Steps);
		auto newValue = params.Min + (stepNum * dStep);
		return newValue - initValue;
	}

	auto initDrag = GuiSliderParams::SLIDER_VERTICAL == params.Orientation ?
		initDragPos.Y :
		initDragPos.X;

	auto newDragPos = GuiSliderParams::SLIDER_VERTICAL == params.Orientation ?
		Position2d{
			params.DragControlOffset.X,
			((int)round(dragFrac * CalcDragLength(params))) + params.DragControlOffset.Y
	} :
		Position2d{
			((int)round(dragFrac * CalcDragLength(params))) + params.DragControlOffset.X,
			params.DragControlOffset.Y
	};

	auto dDragPos = newDragPos - initDragPos;
	auto dDrag = GuiSliderParams::SLIDER_VERTICAL == params.Orientation ?
		dDragPos.Y :
		dDragPos.X;
	auto dDragFrac = ((double)dDrag) / ((double)CalcDragLength(params));

	return valRange * dDragFrac;
}

utils::Position2d GuiSlider::CalcDragPos(GuiSliderParams params,
	double value)
{
	auto valRange = params.Max - params.Min;
	auto valFrac = (value - params.Min) / valRange;

	return GuiSliderParams::SLIDER_VERTICAL == params.Orientation ?
		Position2d{
			params.DragControlOffset.X,
			((int)round(valFrac * CalcDragLength(params))) + params.DragControlOffset.Y
	} :
		Position2d{
			((int)round(valFrac * CalcDragLength(params))) + params.DragControlOffset.X,
			params.DragControlOffset.Y
	};
}

unsigned int GuiSlider::CalcDragLength(GuiSliderParams params)
{
	return GuiSliderParams::SLIDER_VERTICAL == params.Orientation ?
		params.Size.Height - params.DragControlSize.Height - (2 * params.DragGap.Height) :
		params.Size.Width - params.DragControlSize.Width - (2 * params.DragGap.Width);
}
