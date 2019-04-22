#include "GuiModel.h"

using namespace base;
using namespace gui;

GuiModel::GuiModel(GuiModelParams params) :
	GuiElement(params),
	_modelParams(params)
{
}

GuiModel::~GuiModel()
{
}

void GuiModel::Draw(DrawContext& ctx)
{
}
