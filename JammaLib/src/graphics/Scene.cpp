#include "Scene.h"

Scene::Scene(int width, int height) :
	_image(std::make_unique<Image>()),
	_width(width),
	_height(height)
{
}

void Scene::Init(ResourceLib& resourceLib)
{
	_image->Init(resourceLib);

	auto projection = glm::perspective(glm::radians(45.0f), (float)_width / (float)_height, 0.1f, 100.0f);
	auto model = glm::mat4(1.0f);
	_mvp = projection * View() * model;
}

void Scene::Draw(DrawContext& ctx)
{
	auto &glCtx = dynamic_cast<GlDrawContext&>(ctx);
	std::any mvp = _mvp;
	glCtx.SetUniform("MVP", mvp);

	_image->Draw(ctx);
}

bool Scene::Destroy()
{
	_image->Destroy();
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
	return glm::lookAt(glm::vec3(0.f, 0.f, 1.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
}