#pragma once

#include <iostream>
#include <map>
#include <optional>
#include <any>
#include <glm/glm.hpp>
#include <vector>
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

	std::optional<std::any> GetUniform(std::string name);
	void SetUniform(const std::string& name, std::any val);
	
private:
	std::map<std::string, std::any> _uniforms;
};

