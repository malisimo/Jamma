#include "GlDrawContext.h"

GlDrawContext::GlDrawContext()
{
}

GlDrawContext::~GlDrawContext()
{
}

std::optional<int> GlDrawContext::GetUniformInt(std::string name) const
{
	auto it = _uniformsInt.find(name);

	if (it != _uniformsInt.end())
		return std::optional<int>(it->second);

	return std::nullopt;
}

std::optional<float> GlDrawContext::GetUniformFloat(std::string name) const
{
	auto it = _uniformsFloat.find(name);

	if (it != _uniformsFloat.end())
		return std::optional<float>(it->second);

	return std::nullopt;
}

std::optional<glm::mat4> GlDrawContext::GetUniformMat(std::string name) const
{
	auto it = _uniformsMat.find(name);

	if (it != _uniformsMat.end())
		return std::optional<glm::mat4>(it->second);

	return std::nullopt;
}

std::optional<std::any> GlDrawContext::GetUniform(std::string name) const
{
	auto it = _uniforms.find(name);

	if (it != _uniforms.end())
		return std::optional<std::any>(it->second);

	return std::nullopt;
}

void GlDrawContext::SetUniformInt(std::string name, int val)
{
	_uniformsInt[name] = val;
}

void GlDrawContext::SetUniformFloat(std::string name, float val)
{
	_uniformsFloat[name] = val;
}

void GlDrawContext::SetUniformMat(std::string name, glm::mat4 val)
{
	_uniformsMat[name] = val;
}

void GlDrawContext::SetUniform(std::string name, std::any val)
{
	_uniforms[name] = val;
}