#pragma once

#include <string>
#include "../utils/CommonTypes.h"

namespace base
{
	class DrawContext
	{
	public:
		DrawContext() :
			_size({ 1,1 }) {}

		enum ContextType { DEFAULT, OPENGL };
		virtual auto GetContextType() -> ContextType
		{
			return DEFAULT;
		}

		void SetSize(utils::Size2d size)
		{
			_size = size;
		}

	protected:
		utils::Size2d _size;
	};
}
