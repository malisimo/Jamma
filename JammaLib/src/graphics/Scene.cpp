#include "Scene.h"

Scene::Scene(int width, int height) :
	_image(Image()),
	_width(width),
	_height(height)
{
}

void Scene::Init()
{
	_image.Init();

	glm::mat4 Perspective = glm::perspective(glm::pi<float>() * 0.25f, ((float)_width) / ((float)_height), 0.1f, 100.0f);
	auto Model = glm::mat4(1.0f);
	_mvp = Perspective * View() * Model;
}

void Scene::Draw(const DrawContext& ctx)
{
	auto glCtx = dynamic_cast<const GlDrawContext&>(ctx);
	glCtx.SetUniformMat("MVP", _mvp);

	_image.Draw(ctx);
}

bool Scene::Destroy()
{
	_image.Destroy();
	return true;
}

int Scene::Width() const
{
	return _width;
}

int Scene::Height() const
{
	return _height;
}

glm::mat4 Scene::View()
{
	return glm::lookAt(glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
}