#include "Image.h"

using namespace base;
using namespace graphics;
using namespace utils;
using namespace resources;

Image::Image(ImageParams params) :
	Drawable(params),
	Sizeable(params),
	_vertexArray(0),
	_vertexBuffer{ 0,0 },
	_shaderName(params.Shader),
	_texture(std::weak_ptr<TextureResource>()),
	_shader(std::weak_ptr<ShaderResource>())
{
}

void Image::SetSize(Size2d size)
{
	ReleaseResources();

	Sizeable::SetSize(size);

	InitVertexArray();
}

void Image::Draw(DrawContext& ctx)
{
	auto texture = _texture.lock();
	auto shader = _shader.lock();

	if (!texture || !shader)
		return;

	glUseProgram(shader->GetId());
	shader->SetUniforms(dynamic_cast<GlDrawContext&>(ctx));

	glBindVertexArray(_vertexArray);

	glBindTexture(GL_TEXTURE_2D, texture->GetId());

	//glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 1);
	glDrawArrays(GL_TRIANGLES, 0, VertexCount);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

bool Image::InitResources(ResourceLib& resourceLib)
{
	auto validated = true;

	if (validated)
		validated = InitTexture(resourceLib);
	if (validated)
		validated = InitShader(resourceLib);
	if (validated)
		validated = InitVertexArray();

	return validated && GlUtils::CheckError("Image::Init()");
}

bool Image::ReleaseResources()
{
	glDeleteBuffers(2, _vertexBuffer);
	_vertexBuffer[0] = 0;
	_vertexBuffer[1] = 0;

	glDeleteVertexArrays(1, &_vertexArray);
	_vertexArray = 0;

	return true;
}

bool Image::InitTexture(ResourceLib& resourceLib)
{
	auto textureOpt = resourceLib.GetResource(_drawParams.Texture);
	
	if (!textureOpt.has_value())
		return false;

	auto resource = textureOpt.value().lock();

	if (!resource)
		return false;

	if (TEXTURE != resource->GetType())
		return false;
	
	_texture = std::dynamic_pointer_cast<TextureResource>(resource);

	return true;
}

bool Image::InitShader(ResourceLib& resourceLib)
{
	auto shaderOpt = resourceLib.GetResource(_shaderName);

	if (!shaderOpt.has_value())
		return false;

	auto resource = shaderOpt.value().lock();

	if (!resource)
		return false;

	if (SHADER != resource->GetType())
		return false;

	_shader = std::dynamic_pointer_cast<ShaderResource>(resource);

	return true;
}

bool Image::InitVertexArray()
{
	glGenVertexArrays(1, &_vertexArray);
	glBindVertexArray(_vertexArray);

	glGenBuffers(2, _vertexBuffer);

	GLfloat verts[] = {
		0.0f, 0.0f, 0.0f,
		(float)_sizeParams.Size.Width, 0.0f, 0.0f,
		0.0f,  (float)_sizeParams.Size.Height, 0.0f,
		0.0f,  (float)_sizeParams.Size.Height, 0.0f,
		(float)_sizeParams.Size.Width, 0.0f, 0.0f,
		(float)_sizeParams.Size.Width, (float)_sizeParams.Size.Height, 0.0f,
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
