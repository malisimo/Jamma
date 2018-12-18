#pragma once

#include <map>
#include <optional>
#include <any>
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

	std::optional<int> GetUniformInt(std::string name) const;
	std::optional<float> GetUniformFloat(std::string name) const;
	std::optional<glm::mat4> GetUniformMat(std::string name) const;

	std::optional<std::any> GetUniform(std::string name) const;

	void SetUniformInt(std::string name, int val);
	void SetUniformFloat(std::string name, float val);
	void SetUniformMat(std::string name, glm::mat4 val);

	void SetUniform(std::string name, std::any val);

private:
	std::map<std::string, int> _uniformsInt;
	std::map<std::string, float> _uniformsFloat;
	std::map<std::string, glm::mat4> _uniformsMat;

	std::map<std::string, std::any> _uniforms;
};

