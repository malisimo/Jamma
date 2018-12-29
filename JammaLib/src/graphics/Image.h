#pragma once

#include <array>
#include <memory>
#include <gl/glew.h>
#include <gl/gl.h>
#include "gl/glext.h"
#include "gl/wglext.h"
#include "glm/glm.hpp"

#include "Drawable.h"
#include "../resources/ResourceLib.h"
#include "GlUtils.h"

class Image :
	public Drawable
{
public:
	Image();
	~Image() { Destroy(); }

	// Copy
	Image(const Image &) = delete;
	Image& operator=(const Image &) = delete;

	// Move
	Image(Image &&other) :
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
			Destroy();
			std::swap(_vertexArray, other._vertexArray);
			std::swap(_vertexBuffer, other._vertexBuffer);
			_texture.swap(other._texture);
			_shader.swap(other._shader);
		}

		return *this;
	}

public:
	virtual bool Init(ResourceLib& resourceLib);
	virtual void Draw(DrawContext& ctx);
	virtual bool Destroy();
	
private:
	const int VertexCount = 6;

	int _width;
	int _height;

	GLuint _vertexArray;
	GLuint _vertexBuffer[2];
	std::weak_ptr<TextureResource> _texture;
	std::weak_ptr<ShaderResource> _shader;

private:
	bool InitTexture(ResourceLib& resourceLib);
	bool InitShader(ResourceLib& resourceLib);
	bool InitVertexArray();
};

