#include "Scene.h"

Scene::Scene(int width, int height) :
	_width(width),
	_height(height),
	_viewProj(glm::mat4()),
	_overlayViewProj(glm::mat4()),
	_label(std::make_unique<GuiLabel>("Hello")),
	_image(std::make_unique<Image>())
{
}

void Scene::Init(ResourceLib& resourceLib)
{
	_label->Init(resourceLib);
	_image->Init(resourceLib);

	auto ar = _height > 0 ? (float)_width / (float)_height : 1.0f;
	auto projection = glm::perspective(glm::radians(45.0f), ar, 0.1f, 100.0f);
	_viewProj = projection * View();

	auto hScale = _width > 0 ? 2.0f / (float)_width : 1.0f;
	auto vScale = _height > 0 ? 2.0f / (float)_height : 1.0f;
	_overlayViewProj = glm::mat4(1.0);
	_overlayViewProj = glm::translate(_overlayViewProj, glm::vec3(-1.0f, -1.0f, -1.0f));
	_overlayViewProj = glm::scale(_overlayViewProj, glm::vec3(hScale, vScale, 1.0f));
}

void Scene::Draw(DrawContext& ctx)
{
	std::vector<glm::mat4> mvp;

	// Draw scene
	auto &glCtx = dynamic_cast<GlDrawContext&>(ctx);
	mvp.clear();
	mvp.push_back(_viewProj);
	glCtx.SetUniform("MVP", mvp);

	_image->Draw(ctx);

	// Draw overlays
	mvp.clear();
	mvp.push_back(_overlayViewProj);
	glCtx.SetUniform("MVP", mvp);

	_label->Draw(ctx);
}

bool Scene::Release()
{
	_image->Release();
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