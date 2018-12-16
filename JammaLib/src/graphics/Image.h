#pragma once

#include <array>
#include <iostream>
#include <gl/glew.h>
#include <gl/gl.h>
#include "gl/glext.h"
#include "gl/wglext.h"
#include "glm/glm.hpp"

#include "Drawable.h"
#include "Shader.h"

class Image :
	public Drawable
{
public:
	Image();
	~Image() { Destroy(); }

	// Delete the copy constructor/assignment
	Image(const Image &) = delete;
	Image &operator=(const Image &) = delete;

	Image(Image &&other) :
		_texture(other._texture),
		_textureName(other._textureName),
		_vertexArray(other._vertexArray),
		_vertexBuffer(other._vertexBuffer),
		_vertShader(other._vertShader),
		_fragShader(other._fragShader)
	{
		other._texture = 0;
		other._textureName = 0;
		other._vertexArray = 0;
		other._vertexBuffer = 0;
		other._vertShader = Shader();
		other._fragShader = Shader();
	}

	Image &operator=(Image &&other)
	{
		if (this != &other)
		{
			Destroy();
			std::swap(_texture, other._texture);
			std::swap(_textureName, other._textureName);
			std::swap(_vertexArray, other._vertexArray);
			std::swap(_vertexBuffer, other._vertexBuffer);
			std::swap(_vertShader, other._vertShader);
			std::swap(_fragShader, other._fragShader);
		}
	}

public:
	virtual bool Init();
	virtual void Draw(const DrawContext& ctx);
	virtual bool Destroy();
	
private:
	const int VertexCount = 4;

	int _width;
	int _height;

	GLuint _texture;
	GLuint _textureName;
	GLuint _vertexArray;
	GLuint _vertexBuffer;

	Shader _vertShader;
	Shader _fragShader;

private:
	bool InitShader();
	bool InitTexture();
	bool InitVertexArray();

	unsigned char* LoadTexture();
	bool CheckError(std::string log);
};

