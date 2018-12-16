#include "Image.h"

Image::Image() :
	_width(1024),
	_height(768)
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

	return Validated && CheckError("Image::Init()");
}

void Image::Draw(const DrawContext& ctx)
{
	_vertShader.Activate(dynamic_cast<const GlDrawContext&>(ctx));
	_fragShader.Activate(dynamic_cast<const GlDrawContext&>(ctx));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glBindVertexArray(_vertexArray);

	glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 1);
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

	glDeleteVertexArrays(1, &_vertexArray);
	_vertexArray = 0;
	_vertexBuffer = 0;

	return true;
}

bool Image::InitShader()
{
	auto res = true; 
	
	{
		auto shader = Shader::FromFile("./shaders/texture.vert", GL_VERTEX_SHADER);
		if (shader.has_value())
		{
			_vertShader = shader.value();
			res &= _vertShader.Init();
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
			res &= _fragShader.Init();
		}
		else
		{
			res = false;
			_fragShader = Shader();
		}
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
	static const GLfloat verts[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};

	glGenVertexArrays(1, &_vertexArray);
	glBindVertexArray(_vertexArray);

	glGenBuffers(1, &_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	//glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

unsigned char* Image::LoadTexture()
{
	auto pixels = new unsigned char[_width * _height * 4];

	for (size_t col = 0; col < _width; col++)
	{
		for (size_t row = 0; row < _height; row++)
		{
			pixels[row*_width + col] = 19 * ((row * _width + col) % 13);
		}
	}

	return pixels;
}

bool Image::CheckError(std::string log)
{
	bool foundError = false;

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		foundError = true;
		std::cout << err << std::endl;
	}

	if (foundError)
		std::cout << log << std::endl;

	return !foundError;
}
