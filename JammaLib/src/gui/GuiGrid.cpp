#include "GuiGrid.h"


GuiGrid::GuiGrid(GuiGridParams guiParams) :
	GuiElement(guiParams)
{
}

GuiGrid::~GuiGrid()
{
}

std::vector<unsigned int> GetCellSizes(std::vector<Cell> cells)
{
	//auto numAutoSize = std::count(cells.begin(), cells.end(), [](Cell cell) {return cell.Resizeable; });
	return {};
}

void GuiGrid::UpdateChildren()
{
	auto x = 0;
	auto y = 0;
	auto w = 1;
	auto h = 1;

	for (auto& child : _children)
	{
		//child.SetPos(x, y);
		//child.SetSize(w, h);
	}
}
