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
	~Image();

	virtual void Draw(const DrawContext& ctx);
	
private:
	const int VertexCount = 4;

	int _width;
	int _height;

	GLuint _texture;
	GLuint _textureName;
	GLuint _vertexArray;
	GLuint _vertexBuffer;

	Shader _shader;

private:
	bool Init();
	bool Destroy();
	bool InitTexture();
	bool InitShader();
	bool InitVertexArray();

	bool CheckError(std::string log);
};

