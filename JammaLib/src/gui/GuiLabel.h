#pragma once

#include <memory>
#include "GuiElement.h"
#include "Drawable.h"
#include "../graphics/Font.h"
#include "../resources/ResourceLib.h"

class GuiLabelParams : public GuiElementParams
{
public:
	GuiLabelParams(GuiElementParams params,
		std::string string) :
		GuiElementParams(params),
		String(string)
	{}

public:
	std::string String;
};

class GuiLabel :
	public GuiElement
{
public:
	GuiLabel(GuiLabelParams guiParams);
	~GuiLabel() { ReleaseResources(); }

public:
	virtual void Draw(DrawContext& ctx) override;
	virtual bool InitResources(ResourceLib& resourceLib) override;
	virtual bool ReleaseResources() override;

private:
	bool InitVertexArray();

private:
	std::string _str;
	GLuint _vertexArray;
	std::weak_ptr<TextureResource> _texture;
	std::weak_ptr<ShaderResource> _shader;
	std::weak_ptr<Font> _font;
};
