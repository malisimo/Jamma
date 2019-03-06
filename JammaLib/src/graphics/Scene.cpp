#include "Scene.h"

Scene::Scene(SceneParams params) :
	Drawable(params),
	Sizeable(params),
	Audible(params),
	_viewProj(glm::mat4()),
	_overlayViewProj(glm::mat4()),
	_label(std::unique_ptr<GuiLabel>()),
	_slider(std::unique_ptr<GuiSlider>()),
	_image(std::make_unique<Image>(ImageParams(DrawableParams{ "grid" }, SizeableParams{ 450, 450 }, "texture"))),
	_audioDevice(std::unique_ptr<AudioDevice>())
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
	sliderParams.Size = { 40,342 };
	sliderParams.MinSize = { 40,342 };
	sliderParams.DragLength = 300;
	sliderParams.DragControlOffset = { 4,5 };
	sliderParams.DragControlSize = { 32,32 };
	sliderParams.Texture = "fader_back";
	sliderParams.DragTexture = "fader";
	sliderParams.DragOverTexture = "fader_over";
	_slider = std::make_unique<GuiSlider>(sliderParams);

	LoopParams loopParams;
	loopParams.Wav = "hh";
	_loop = std::make_unique<Loop>(loopParams);

	_audioDevice = std::make_unique<AudioDevice>();
}

void Scene::Draw(DrawContext& ctx)
{
	// Draw scene
	auto &glCtx = dynamic_cast<GlDrawContext&>(ctx);
	glCtx.ClearMvp();
	glCtx.PushMvp(_viewProj);

	//_image->Draw(ctx);
	glCtx.PopMvp();

	// Draw overlays
	glCtx.ClearMvp();
	glCtx.PushMvp(_overlayViewProj);

	_slider->Draw(ctx);
	_label->Draw(ctx);

	glCtx.PopMvp();
}

bool Scene::InitResources(ResourceLib& resourceLib)
{
	_label->InitResources(resourceLib);
	_slider->InitResources(resourceLib);
	_image->InitResources(resourceLib);
	_loop->InitResources(resourceLib);

	InitSize();

	return Drawable::InitResources(resourceLib);
}

bool Scene::ReleaseResources()
{
	_label->ReleaseResources();
	_slider->ReleaseResources();
	_image->ReleaseResources();
	_loop->ReleaseResources();

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

void Scene::Play(float* buf, unsigned int numChans, unsigned int numSamps)
{
	_loop->Play(buf, numChans, numSamps);
}

void Scene::InitAudio()
{
	auto dev = AudioDevice::Open(Scene::OnAudio,
		[](RtAudioError::Type type, const std::string& err) { std::cout << "[" << type << " RtAudio Error] " << err << std::endl; },
		this);

	if (dev.has_value())
	{
		_audioDevice = std::move(dev.value());
		_audioDevice->Start();
	}
}

RtAudio::DeviceInfo Scene::AudioInputDeviceInfo()
{
	if (_audioDevice)
		return _audioDevice->GetInputStreamInfo();

	return RtAudio::DeviceInfo();
}

RtAudio::DeviceInfo Scene::AudioOutputDeviceInfo()
{
	if (_audioDevice)
		return _audioDevice->GetOutputStreamInfo();

	return RtAudio::DeviceInfo();
}

int Scene::OnAudio(void *outBuffer, void *inBuffer, unsigned int numSamps, double streamTime, RtAudioStreamStatus status, void *userData)
{
	float* inBuf = (float*)inBuffer;
	float* outBuf = (float*)outBuffer;
	Scene* scene = (Scene*)userData;
	RtAudio::DeviceInfo inDeviceInfo;
	RtAudio::DeviceInfo outDeviceInfo;

	if (scene)
		outDeviceInfo = scene->AudioOutputDeviceInfo();

	auto numChannels = outDeviceInfo.outputChannels;

	for (unsigned int samp = 0; samp < numSamps; samp++)
	{
		for (unsigned int chan = 0; chan < numChannels; chan++)
		{
			outBuf[samp * numChannels + chan] = 0.0f;
		}
	}

	scene->Play(outBuf, outDeviceInfo.outputChannels, numSamps);

	return 0;
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