#pragma once
#include <string>

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

