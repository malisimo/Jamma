#pragma once

#include "Action.h"

namespace actions
{
	class CursorAction :
		public base::Action
	{
	public:
		CursorAction();
		~CursorAction();

	public:
		enum CursorActionType
		{
			CAPTURE_CHANGED,
			SET_CURSOR
		};

		unsigned int Cursor;
	};
}
