#include "GlDrawContext.h"

using namespace graphics;
using namespace utils;

GlDrawContext::GlDrawContext() :
	DrawContext(),
	_modelView({}),
	_projection(glm::mat4(1.0))
{
}

GlDrawContext::~GlDrawContext()
{
}

std::optional<std::any> GlDrawContext::GetUniform(std::string name)
{
	if (_MvpUniformName == name)
	{
		auto collapsed = _projection;
		for (auto m : _modelView)
		{
			collapsed *= m;
		}

		return (name, collapsed);
	}

	auto it = _uniforms.find(name);

	if (it != _uniforms.end())
		return it->second;

	return std::nullopt;
}

void GlDrawContext::SetProjection(const glm::mat4 mat) noexcept
{
	_projection = mat;
}

void GlDrawContext::SetUniform(const std::string& name, std::any val)
{
	_uniforms[name] = val;
}

void GlDrawContext::PushModelView(const glm::mat4 mat) noexcept
{
	_modelView.push_back(mat);
}

void GlDrawContext::PopModelView() noexcept
{
	if (!_modelView.empty())
		_modelView.pop_back();
}

void GlDrawContext::ClearModelView() noexcept
{
	_modelView.clear();
}

Position2d GlDrawContext::ProjectScreen(utils::Position3d pos)
{
	auto p = glm::vec3(pos.X, pos.Y, pos.Z);
	auto collapsed = _projection;
	for (auto m : _modelView)
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