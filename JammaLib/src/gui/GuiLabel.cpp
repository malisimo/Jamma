#include "GuiLabel.h"


GuiLabel::GuiLabel(const std::string& str) :
	_str(str),
	_vertexArray(0),
	_texture(std::weak_ptr<TextureResource>()),
	_shader(std::weak_ptr<ShaderResource>()),
	_font(std::weak_ptr<Font>())
{
}

bool GuiLabel::Init(ResourceLib& resourceLib)
{
	auto fontOpt = resourceLib.GetFont(FontOptions::FONT_TINY);

	if (!fontOpt.has_value())
		return false;

	_font = fontOpt.value();

	auto validated = InitVertexArray();

	return validated && GlUtils::CheckError("GuiLabel::Init()");
}

void GuiLabel::Draw(DrawContext& ctx)
{
	auto font = _font.lock();

	if (font)
		font->Draw(dynamic_cast<GlDrawContext&>(ctx), _vertexArray, (unsigned int)_str.size());
}

bool GuiLabel::Release()
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