#pragma once
#include "DrawContext.h"

class GlDrawContext :
	public DrawContext
{
public:
	GlDrawContext();
	~GlDrawContext();

public:
	auto GetContextType() -> DrawContext::ContextType
	{
		return ContextType::OPENGL;
	}

	glm::mat4 GetMvp() const;
	void SetMvp(glm::mat4 mat);

private:
	glm::mat4 _mvp;
};

