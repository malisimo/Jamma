#include "GlDrawContext.h"

GlDrawContext::GlDrawContext()
{
}

GlDrawContext::~GlDrawContext()
{
}

std::optional<std::any> GlDrawContext::GetUniform(std::string name)
{
	auto it = _uniforms.find(name);

	if (it != _uniforms.end())
		return std::optional<std::any>(it->second);

	return std::nullopt;
}

void GlDrawContext::SetUniform(const std::string& name, std::any val)
{
	_uniforms[name] = val;
}
