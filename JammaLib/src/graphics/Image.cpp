#include "Image.h"

Image::Image() :
	_width(1024),
	_height(768),
	_shader(Shader())
{
	auto shader = Shader::FromFile("shaders/texture.glsl", 0);

	if (shader.has_value())
		_shader = shader.value();
}

Image::~Image()
{
	Destroy();
}

void Image::Draw(const DrawContext& ctx)
{
	_shader.Activate(dynamic_cast<const GlDrawContext&>(ctx));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glBindVertexArray(_vertexArray);

	glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 1);
}

bool Image::Init()
{
	bool Validated = true;

	if (Validated)
		Validated = _shader.Init();
	if (Validated)
		Validated = InitTexture();
	if (Validated)
		Validated = InitVertexArray();

	return Validated && CheckError("Image::Init()");
}

bool Image::Destroy()
{
	_shader.Destroy();

	glDeleteTextures(1, &_texture);
	glDeleteVertexArrays(1, &_vertexArray);

	return true;
}


bool Image::InitTexture()
{
	glGenTextures(1, &_texture);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 768, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

bool Image::InitShader()
{
	return _shader.Init();
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

bool Image::CheckError(std::string log)
{
	bool foundError;

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		foundError = true;
		std::cout << err << std::endl;
	}

	if (foundError)
		std::cout << log << std::endl;

	return foundError;
}
