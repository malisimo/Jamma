#pragma once

#include <algorithm>
#include <functional>
#include "../utils/CommonTypes.h"
#include "../base/GuiElement.h"
#include "../resources/TextureResource.h"

class GuiSliderParams;

class GuiSlider : public GuiElement
{
public:
	GuiSlider(GuiSliderParams guiParams);
	~GuiSlider();

public:
	double Value() const;

	virtual bool HitTest(Position2d pos) override;
	virtual void Draw(DrawContext& ctx) override;
	virtual void OnAction(TouchAction action) override;
	virtual void OnAction(TouchMoveAction action) override;
	virtual bool InitResources(ResourceLib& resourceLib) override;
	virtual bool ReleaseResources() override;

private:
	std::function<double(Position2d)> _calcValueFun;
	std::function<Position2d(double)> _calcDragPosFun;

private:
	bool _isDragging;
	Position2d _initClickPos;
	Position2d _initDragPos;
	GuiElement _dragElement;
};

class GuiSliderParams : public GuiElementParams
{
public:
	GuiSliderParams() :
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
	Position2d DragControlOffset;
	Size2d DragControlSize;
};
