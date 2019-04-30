#include "Scene.h"
#include "glm/ext.hpp"

using namespace base;
using namespace actions;
using namespace audio;
using namespace engine;
using namespace gui;
using namespace graphics;
using namespace resources;
using namespace utils;

Scene::Scene(SceneParams params) :
	Drawable(params),
	Sizeable(params),
	_viewProj(glm::mat4()),
	_overlayViewProj(glm::mat4()),
	_channelMixer(ChannelMixerParams{}),
	_label(std::unique_ptr<GuiLabel>()),
	_slider(std::unique_ptr<GuiSlider>()),
	_image(std::make_unique<Image>(
		ImageParams(
			DrawableParams{ "grid" },
			SizeableParams{ 450, 450 },
			"texture"))),
	_audioDevice(std::unique_ptr<AudioDevice>()),
	_stations()
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
	auto take = std::make_unique<LoopTake>(DrawableParams());
	take->AddLoop(std::make_unique<Loop>(loopParams));
	
	auto station = std::make_shared<Station>(DrawableParams());
	station->AddTake(std::move(take));

	_stations.push_back(std::move(station));

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

	for (auto& station : _stations)
		station->InitResources(resourceLib);

	InitSize();

	return Drawable::InitResources(resourceLib);
}

bool Scene::ReleaseResources()
{
	_label->ReleaseResources();
	_slider->ReleaseResources();
	_image->ReleaseResources();

	for (auto& station : _stations)
		station->ReleaseResources();

	return Drawable::ReleaseResources();
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

void Scene::InitAudio()
{
	auto dev = AudioDevice::Open(Scene::AudioCallback,
		[](RtAudioError::Type type, const std::string& err) { std::cout << "[" << type << " RtAudio Error] " << err << std::endl; },
		this);

	if (dev.has_value())
	{
		_audioDevice = std::move(dev.value());

		auto inParams = _audioDevice->GetInputStreamInfo();
		auto outParams = _audioDevice->GetOutputStreamInfo();
		
		_channelMixer.SetParams(ChannelMixerParams({
				ChannelMixer::DefaultBufferSize,
				ChannelMixer::DefaultBufferSize,
				inParams.inputChannels,
				outParams.outputChannels}));

		_audioDevice->Start();
	}
}

int Scene::AudioCallback(void* outBuffer,
	void* inBuffer,
	unsigned int numSamps,
	double streamTime,
	RtAudioStreamStatus status,
	void* userData)
{
	Scene* scene = (Scene*)userData;
	scene->OnAudio((float*)inBuffer, (float*)outBuffer, numSamps);

	return 0;
}

void Scene::OnAudio(float* inBuf, float* outBuf, unsigned int numSamps)
{
	if (nullptr != inBuf)
	{
		auto inDeviceInfo = nullptr == _audioDevice ? RtAudio::DeviceInfo() : _audioDevice->GetInputStreamInfo();
		_channelMixer.FromAdc(inBuf, inDeviceInfo.inputChannels, numSamps);
	}

	if (nullptr != outBuf)
	{
		auto outDeviceInfo = nullptr == _audioDevice ? RtAudio::DeviceInfo() : _audioDevice->GetOutputStreamInfo();
		std::fill(outBuf, outBuf + numSamps * outDeviceInfo.outputChannels, 0.0f);

		for (auto& station : _stations)
			_channelMixer.Play(station, numSamps);

		_channelMixer.ToDac(outBuf, outDeviceInfo.outputChannels, numSamps);
	}
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