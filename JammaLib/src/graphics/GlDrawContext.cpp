#include "GlDrawContext.h"

using namespace graphics;
using namespace utils;

GlDrawContext::GlDrawContext() :
	DrawContext()
{
}

GlDrawContext::~GlDrawContext()
{
}

std::optional<std::any> GlDrawContext::GetUniform(std::string name)
{
	if (_MvpUniformName == name)
		return (name, _mvp);

	auto it = _uniforms.find(name);

	if (it != _uniforms.end())
		return it->second;

	return std::nullopt;
}

void GlDrawContext::SetUniform(const std::string& name, std::any val)
{
	_uniforms[name] = val;
}

void GlDrawContext::PushMvp(const glm::mat4 mat) noexcept
{
	_mvp.push_back(mat);
}

void GlDrawContext::PopMvp() noexcept
{
	if (!_mvp.empty())
		_mvp.pop_back();
}

void GlDrawContext::ClearMvp() noexcept
{
	_mvp.clear();
}

Position2d GlDrawContext::ProjectScreen(utils::Position3d pos)
{
	auto p = glm::vec3(pos.X, pos.Y, pos.Z);
	auto collapsed = glm::mat4(1.0);
	for (auto m : _mvp)
	{
		collapsed *= m;
	}

	auto screenPosHom = collapsed * glm::vec4{ pos.X, pos.Y, pos.Z, 1.0 };
	auto screenPosNorm = glm::vec3{ screenPosHom.x / screenPosHom.w,
		screenPosHom.y / screenPosHom.w,
		screenPosHom.z / screenPosHom.w };

	auto screenPosPix = Position2d{
		(int)((screenPosNorm.x + 1.0) * 0.5 * _size.Width),
		(int)((screenPosNorm.y + 1.0) * 0.5 * _size.Height) };

	return screenPosPix;
}