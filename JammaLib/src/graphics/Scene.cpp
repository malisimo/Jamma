#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Init()
{
	glm::mat4 Perspective = glm::perspective(glm::pi<float>() * 0.25f, ((float)_width) / ((float)_height), 0.1f, 100.0f);
	auto Model = glm::mat4(1.0f);
	MVP = Perspective * View() * Model;
}

glm::mat4 Scene::View()
{
	return glm::lookAt(glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
}

void Scene::Draw(const DrawContext& ctx)
{
	auto glCtx = dynamic_cast<const GlDrawContext&>(ctx);
	glCtx.SetMvp(MVP);
}