#include "GuiLabel.h"

using namespace base;
using namespace gui;
using namespace graphics;
using namespace resources;

GuiLabel::GuiLabel(GuiLabelParams guiParams) :
	GuiElement(guiParams),
	_str(guiParams.String),
	_vertexArray(0),
	_texture(std::weak_ptr<TextureResource>()),
	_shader(std::weak_ptr<ShaderResource>()),
	_font(std::weak_ptr<Font>())
{
}

void GuiLabel::Draw(DrawContext& ctx)
{
	auto model = glm::mat4(1.0);
	model = glm::translate(glm::mat4(1.0), glm::vec3(100.f, 200.f, 0.f));
	auto glCtx = dynamic_cast<GlDrawContext&>(ctx);
	auto mvpOpt = glCtx.GetUniform("MVP");

	if (mvpOpt.has_value())
	{
		auto mvp = std::any_cast<std::vector<glm::mat4>>(mvpOpt.value());
		mvp.push_back(model);
		glCtx.SetUniform("MVP", mvp);
	}

	auto font = _font.lock();

	if (font)
		font->Draw(glCtx, _vertexArray, (unsigned int)_str.size());
}

bool GuiLabel::InitResources(ResourceLib& resourceLib)
{
	auto fontOpt = resourceLib.GetFont(graphics::FontOptions::FONT_LARGE);

	if (!fontOpt.has_value())
		return false;

	_font = fontOpt.value();

	auto validated = InitVertexArray();

	return validated && utils::GlUtils::CheckError("GuiLabel::Init()");
}

bool GuiLabel::ReleaseResources()
{
	glDeleteVertexArrays(1, &_vertexArray);
	return true;
}

bool GuiLabel::InitVertexArray()
{
	auto font = _font.lock();

	if (!font)
		return false;
		
	_vertexArray = font->InitVertexArray(_str, GL_STATIC_DRAW);
	return true;
}
