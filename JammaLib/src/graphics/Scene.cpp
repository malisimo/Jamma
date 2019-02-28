#include "Scene.h"

Scene::Scene(SceneParams params) :
	Drawable(params),
	Sizeable(params),
	Audible(params),
	_viewProj(glm::mat4()),
	_overlayViewProj(glm::mat4()),
	_label(nullptr),
	_slider(nullptr),
	_image(std::make_unique<Image>(ImageParams(DrawableParams{ "grid" }, SizeableParams{ 450, 450 }, "texture")))
{
	GuiLabelParams labelParams(GuiElementParams(
		DrawableParams{ "" },
		MoveableParams{ 10,10 },
		SizeableParams{ 200,80 },
		"",
		"",
		"",
		{}), "Hello");
	_label = std::make_unique<GuiLabel>(labelParams);

	// Nicer with default constructor
	GuiSliderParams sliderParams;
	sliderParams.Position = { 50,50 };
	sliderParams.Size = { 256, 256 };
	sliderParams.MinSize = { 256, 256 };
	sliderParams.Texture = "fader_back";
	sliderParams.DragControlOffset = { 5,5 };
	sliderParams.DragControlSize = { 32,32 };
	sliderParams.DragTexture = "fader";
	sliderParams.DragOverTexture = "fader_over";
	_slider = std::make_unique<GuiSlider>(sliderParams);
}

void Scene::Draw(DrawContext& ctx)
{
	std::vector<glm::mat4> mvp;

	// Draw scene
	auto &glCtx = dynamic_cast<GlDrawContext&>(ctx);
	mvp.clear();
	mvp.push_back(_viewProj);
	glCtx.SetUniform("MVP", mvp);

	//_image->Draw(ctx);

	// Draw overlays
	mvp.clear();
	mvp.push_back(_overlayViewProj);
	glCtx.SetUniform("MVP", mvp);

	_slider->Draw(ctx);
	_label->Draw(ctx);
}

bool Scene::InitResources(ResourceLib& resourceLib)
{
	_label->InitResources(resourceLib);
	_slider->InitResources(resourceLib);
	_image->InitResources(resourceLib);

	InitSize();

	return Drawable::InitResources(resourceLib);
}

bool Scene::ReleaseResources()
{
	_image->ReleaseResources();
	return Drawable::ReleaseResources();
}

void Scene::SetSize(Size2d size)
{
	_sizeParams.Size = size;

	InitSize();
}

unsigned int Scene::Width() const
{
	return _sizeParams.Size.Width;
}

unsigned int Scene::Height() const
{
	return _sizeParams.Size.Height;
}

void Scene::OnAction(TouchAction touchAction)
{
	std::cout << "Touch action " << touchAction.Touch << " [" << touchAction.State << "] " << touchAction.Index << std::endl;
	_slider->OnAction(touchAction);
}

void Scene::OnAction(TouchMoveAction touchAction)
{
	std::cout << "Touch Move action " << touchAction.Touch << " [" << touchAction.Position.X << "," << touchAction.Position.Y << "] " << touchAction.Index << std::endl;
	_slider->OnAction(touchAction);
}

void Scene::OnAction(KeyAction keyAction)
{
	std::cout << "Key action " << keyAction.KeyActionType << " [" << keyAction.KeyChar << "]" << std::endl;
	//_slider->OnAction(keyAction);
}

void Scene::InitSize()
{
	auto ar = _sizeParams.Size.Height > 0 ?
		(float)_sizeParams.Size.Width / (float)_sizeParams.Size.Height :
		1.0f;
	auto projection = glm::perspective(glm::radians(45.0f), ar, 0.1f, 100.0f);
	_viewProj = projection * View();

	auto hScale = _sizeParams.Size.Width > 0 ? 2.0f / (float)_sizeParams.Size.Width : 1.0f;
	auto vScale = _sizeParams.Size.Height > 0 ? 2.0f / (float)_sizeParams.Size.Height : 1.0f;
	_overlayViewProj = glm::mat4(1.0);
	_overlayViewProj = glm::translate(_overlayViewProj, glm::vec3(-1.0f, -1.0f, -1.0f));
	_overlayViewProj = glm::scale(_overlayViewProj, glm::vec3(hScale, vScale, 1.0f));
}

glm::mat4 Scene::View()
{
	return glm::lookAt(glm::vec3(0.f, 0.f, 1.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
}