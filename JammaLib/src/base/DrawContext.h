#pragma once
#include <string>
#include "glm/glm.hpp"

class DrawContext
{
public:
	DrawContext();
	~DrawContext();

public:
	enum ContextType { DEFAULT, OPENGL };
	virtual auto GetContextType() -> ContextType
	{
		return DEFAULT;
	}
};

