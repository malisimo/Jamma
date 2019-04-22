#pragma once

#include <vector>
#include <algorithm>
#include "GuiElement.h"

namespace gui
{
	class GuiGridParams : public base::GuiElementParams {};

	class Cell
	{
	public:
		bool Resizeable;
		unsigned int Size;
	};

	class GuiGrid :
		public base::GuiElement
	{
	public:
		GuiGrid(GuiGridParams guiParams);
		~GuiGrid();

		enum Orientation
		{
			HORIZONTAL,
			VERTICAL
		};

	public:
		static std::vector<unsigned int> GetCellSizes(std::vector<Cell> cells);

	protected:
		//virtual void UpdateChildren() override;

	protected:
		Orientation _orientation;
		std::vector<Cell> _cells;
	};
}
