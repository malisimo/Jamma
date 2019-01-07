#pragma once

#include <memory>
#include "Drawable.h"
#include "../graphics/Font.h"
#include "../resources/ResourceLib.h"

class GuiLabel : public Drawable
{
public:
	GuiLabel(const std::string& str);
	~GuiLabel() { Release(); }

public:
	bool Init(ResourceLib& resourceLib);
	virtual void Draw(DrawContext& ctx) override;
	virtual bool Release() override;

private:
	bool InitVertexArray();

private:
	std::string _str;
	GLuint _vertexArray;
	std::weak_ptr<TextureResource> _texture;
	std::weak_ptr<ShaderResource> _shader;
	std::weak_ptr<Font> _font;
};

