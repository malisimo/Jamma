#pragma once

#include "Action.h"
#include "CommonTypes.h"

namespace actions
{
	class WindowAction :
		public base::Action
	{
	public:
		WindowAction();
		~WindowAction();

	public:
		enum WindowEventType
		{
			SIZE,
			SIZE_MINIMISE,
			SIZE_MAXIMISE,
			ENTER_SIZE_MOVE,
			EXIT_SIZE_MOVE,
			DESTROY,
		};

		WindowEventType WindowEventType;
		utils::Position2d Position;
		utils::Size2d Size;
	};
}
