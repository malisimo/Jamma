#include "GuiModel.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

using namespace base;
using namespace gui;
using namespace resources;
using namespace utils;
using graphics::GlDrawContext;

GuiModel::GuiModel(GuiModelParams params) :
	GuiElement(params),
	_modelParams(params)
{
}

void GuiModel::Draw3d(DrawContext& ctx)
{
	auto& glCtx = dynamic_cast<GlDrawContext&>(ctx);
	auto pos = ModelPosition();
	auto scale = ModelScale();
	glCtx.PushMvp(glm::translate(glm::mat4(1.0), glm::vec3(pos.X, pos.Y, pos.Z)));
	glCtx.PushMvp(glm::scale(glm::mat4(1.0), glm::vec3(scale, scale, scale)));

	auto texture = _modelTexture.lock();
	auto shader = _modelShader.lock();

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

	for (auto& child : _children)
		child->Draw3d(ctx);

	glCtx.PopMvp();
}

bool GuiModel::_InitResources(ResourceLib& resourceLib)
{
	auto validated = true;

	if (validated)
		validated = InitTexture(resourceLib);
	if (validated)
		validated = InitShader(resourceLib);
	if (validated)
		validated = InitVertexArray();

	return validated && GlUtils::CheckError("GuiModel::Init()");
}

bool GuiModel::_ReleaseResources()
{
	glDeleteBuffers(2, _vertexBuffer);
	_vertexBuffer[0] = 0;
	_vertexBuffer[1] = 0;

	glDeleteVertexArrays(1, &_vertexArray);
	_vertexArray = 0;

	return true;
}

bool GuiModel::InitTexture(ResourceLib& resourceLib)
{
	auto textureOpt = resourceLib.GetResource(_modelParams.ModelTexture);

	if (!textureOpt.has_value())
		return false;

	auto resource = textureOpt.value().lock();

	if (!resource)
		return false;

	if (TEXTURE != resource->GetType())
		return false;

	_modelTexture = std::dynamic_pointer_cast<TextureResource>(resource);

	return true;
}

bool GuiModel::InitShader(ResourceLib & resourceLib)
{
	auto shaderOpt = resourceLib.GetResource(_modelParams.ModelShader);

	if (!shaderOpt.has_value())
		return false;

	auto resource = shaderOpt.value().lock();

	if (!resource)
		return false;

	if (SHADER != resource->GetType())
		return false;

	_modelShader = std::dynamic_pointer_cast<ShaderResource>(resource);

	return true;
}

bool GuiModel::InitVertexArray()
{
	glGenVertexArrays(1, &_vertexArray);
	glBindVertexArray(_vertexArray);

	glGenBuffers(2, _vertexBuffer);

	GLfloat verts[] = {
		0.0f, 0.0f, 0.0f,
		(float)_sizeParams.Size.Width * 0.5f, (float)_sizeParams.Size.Height * 0.5f, 0.0f,
		0.0f,  (float)_sizeParams.Size.Height, 0.0f,
		(float)_sizeParams.Size.Width,  (float)_sizeParams.Size.Height, 0.0f,
		(float)_sizeParams.Size.Width * 0.5f, (float)_sizeParams.Size.Height * 0.5f, 1.0f,
		(float)_sizeParams.Size.Width, 0.0f, 0.0f,
	};

	glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	static const GLfloat uvs[] = {
		0.0f, 0.0f,
		0.5f, 0.5f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.5f, 0.5f,
		1.0f, 0.0f,
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
