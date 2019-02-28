#pragma once

#include <array>
#include <memory>
#include <gl/glew.h>
#include <gl/gl.h>
#include "gl/glext.h"
#include "gl/wglext.h"
#include "glm/glm.hpp"

#include "Drawable.h"
#include "Sizeable.h"
#include "../resources/ResourceLib.h"
#include "GlUtils.h"

class ImageParams : public DrawableParams, public SizeableParams
{
public:
	ImageParams(DrawableParams drawParams,
		SizeableParams sizeParams,
		std::string shader) :
		DrawableParams(drawParams),
		SizeableParams(sizeParams),
		Shader(shader)
	{}

public:
	std::string Shader;
};

class Image :
	public Drawable,
	public Sizeable
{
public:
	Image(ImageParams drawParams);
	~Image() { ReleaseResources(); }

	// Copy
	Image(const Image &) = delete;
	Image& operator=(const Image &) = delete;

	// Move
	Image(Image &&other) :
		Drawable(other._drawParams),
		Sizeable(other._sizeParams),
		_vertexArray(other._vertexArray),
		_vertexBuffer{ other._vertexBuffer[0], other._vertexBuffer[1] },
		_texture(std::move(other._texture)),
		_shader(std::move(other._shader))
	{
		other._vertexArray = 0;
		other._vertexBuffer[0] = 0;
		other._vertexBuffer[1] = 0;
		other._texture = std::weak_ptr<TextureResource>();
		other._shader = std::weak_ptr<ShaderResource>();
	}

	Image& operator=(Image &&other)
	{
		if (this != &other)
		{
			ReleaseResources();
			std::swap(_vertexArray, other._vertexArray);
			std::swap(_vertexBuffer, other._vertexBuffer);
			_texture.swap(other._texture);
			_shader.swap(other._shader);
		}

		return *this;
	}

public:
	virtual void SetSize(Size2d size) override;
	virtual void Draw(DrawContext& ctx) override;
	virtual bool InitResources(ResourceLib& resourceLib) override;
	virtual bool ReleaseResources() override;
	
private:
	const int VertexCount = 6;

	GLuint _vertexArray;
	GLuint _vertexBuffer[2];
	std::string _shaderName;
	std::weak_ptr<TextureResource> _texture;
	std::weak_ptr<ShaderResource> _shader;

private:
	bool InitTexture(ResourceLib& resourceLib);
	bool InitShader(ResourceLib& resourceLib);
	bool InitVertexArray();
};
