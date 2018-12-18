#include "Image.h"

Image::Image() :
	_width(1024),
	_height(768),
	_texture(0),
	_textureName(0),
	_vertexArray(0),
	_vertexBuffer{ 0,0 },
	_shaderProgram(0)
{
}

bool Image::Init()
{
	bool Validated = true;

	if (Validated)
		Validated = InitShader();
	if (Validated)
		Validated = InitTexture();
	if (Validated)
		Validated = InitVertexArray();

	return Validated && GlUtils::CheckError("Image::Init()");
}

void Image::Draw(const DrawContext& ctx)
{
	glUseProgram(_shaderProgram);

	//_vertShader.Activate(dynamic_cast<const GlDrawContext&>(ctx));
	//_fragShader.Activate(dynamic_cast<const GlDrawContext&>(ctx));

	glBindVertexArray(_vertexArray);
	//glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer[0]);
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);

	//glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 1);
	glDrawArrays(GL_TRIANGLES, 0, VertexCount);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

bool Image::Destroy()
{
	_vertShader.Destroy();
	_vertShader = Shader();
	_fragShader.Destroy();
	_fragShader = Shader();

	glDeleteTextures(1, &_texture);
	_texture = 0;
	_textureName = 0;

	glDeleteBuffers(2, _vertexBuffer);
	_vertexBuffer[0] = 0;
	_vertexBuffer[1] = 0;

	glDeleteVertexArrays(1, &_vertexArray);
	_vertexArray = 0;

	return true;
}

bool Image::InitShader()
{
	auto res = true;

	_shaderProgram = glCreateProgram();
		
	{
		auto shader = Shader::FromFile("./shaders/texture.vert", GL_VERTEX_SHADER);
		if (shader.has_value())
		{
			_vertShader = shader.value();
			glAttachShader(_shaderProgram, _vertShader.Name());
		}
		else
		{
			res = false;
			_vertShader = Shader();
		}
	}

	{
		auto shader = Shader::FromFile("./shaders/texture.frag", GL_FRAGMENT_SHADER);
		if (shader.has_value())
		{
			_fragShader = shader.value();
			glAttachShader(_shaderProgram, _fragShader.Name());
		}
		else
		{
			res = false;
			_fragShader = Shader();
		}
	}

	if (res)
	{
		glLinkProgram(_shaderProgram);

		GLint glResult;
		glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &glResult);

		glUseProgram(_shaderProgram);

		_vertShader.Init(_shaderProgram, { "MVP" });
		_fragShader.Init(_shaderProgram, {});

		glUseProgram(0);
	}

	return res;
}

bool Image::InitTexture()
{
	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_BGRA, GL_UNSIGNED_BYTE, LoadTexture());

	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

bool Image::InitVertexArray()
{
	glGenVertexArrays(1, &_vertexArray);
	glBindVertexArray(_vertexArray);

	glGenBuffers(2, _vertexBuffer);

	static const GLfloat verts[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.5f,  0.5f, 0.0f,
	};

	glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	static const GLfloat uvs[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
	};

	glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), uvs, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	GlUtils::CheckError("Image::InitVertexArray");

	return true;
}

unsigned char* Image::LoadTexture()
{
	auto pixels = new unsigned char[_width * _height * 4];

	for (size_t col = 0; col < _width; col++)
	{
		for (size_t row = 0; row < _height; row++)
		{
			pixels[4*(row*_width + col) + 0] = 19 * ((4*(row * _width + col) + 0) % 13);
			pixels[4*(row*_width + col) + 1] = 19 * ((4*(row * _width + col) + 1) % 13);
			pixels[4*(row*_width + col) + 2] = 19 * ((4*(row * _width + col) + 2) % 13);
			pixels[4*(row*_width + col) + 3] = 19 * ((4*(row * _width + col) + 3) % 13);
		}
	}

	return pixels;
}
