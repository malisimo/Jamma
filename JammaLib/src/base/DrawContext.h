#pragma once

#include <string>

namespace base
{
	class DrawContext
	{
	public:
		enum ContextType { DEFAULT, OPENGL };
		virtual auto GetContextType() -> ContextType
		{
			return DEFAULT;
		}

	public:
		unsigned int Width;
		unsigned int Height;
	};
}
