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
	_channelMixer(std::make_shared<ChannelMixer>(ChannelMixerParams{})),
	_label(std::unique_ptr<GuiLabel>()),
	_audioDevice(std::unique_ptr<AudioDevice>()),
	_masterLoop(std::shared_ptr<Loop>()),
	_stations(),
	_touchDownElement(std::weak_ptr<GuiElement>()),
	_audioCallbackCount(0)
{
	GuiLabelParams labelParams(GuiElementParams(
		DrawableParams{ "" },
		MoveableParams{ 10,10 },
		SizeableParams{ 200,80 },
		"",
		"",
		"",
		{}), "Jamma");
	_label = std::make_unique<GuiLabel>(labelParams);

	// Nicer with default constructor
	GuiSliderParams sliderParams;
	sliderParams.Position = { 2,4 };
	sliderParams.Size = { 40,312 };
	sliderParams.MinSize = { 40,312 };
	sliderParams.DragLength = 270;
	sliderParams.DragControlOffset = { 4,5 };
	sliderParams.DragControlSize = { 32,32 };
	sliderParams.Texture = "fader_back";
	sliderParams.DragTexture = "fader";
	sliderParams.DragOverTexture = "fader_over";

	PanMixBehaviourParams mixBehaviour;
	mixBehaviour.ChannelLevels = { 0.8f, 0.2f };
	AudioMixerParams mixerParams;
	mixerParams.Size = { 160, 320 };
	mixerParams.Position = { 6, 6 };
	mixerParams.SliderParams = sliderParams;
	mixerParams.Behaviour = mixBehaviour;

	StationParams stationParams;
	stationParams.Size = { 240, 200 };
	stationParams.Position = { 120, 45 };
	auto station = std::make_shared<Station>(stationParams);

	LoopParams loopParams;
	loopParams.Wav = "hh";
	loopParams.Size = { 80, 80 };
	loopParams.Position = { 10, 22 };
	loopParams.MixerParams = mixerParams;

	LoopTakeParams takeParams;
	takeParams.Size = { 140, 140 };
	takeParams.Position = { 4, 4 };
	takeParams.Loops = { loopParams };
	station->AddTake(takeParams);

	TriggerParams trigParams;
	trigParams.Size = { 24, 24 };
	trigParams.Position = { 6, 6 };
	trigParams.Activate = {
		DualBinding(
			TriggerBinding{
				TriggerSource::TRIGGER_KEY,
				49,
				1
			},
			TriggerBinding{
				TriggerSource::TRIGGER_KEY,
				49,
				0
			}) };
	trigParams.Ditch = {
		DualBinding(
			TriggerBinding{
				TriggerSource::TRIGGER_KEY,
				50,
				1
			},
			TriggerBinding{
				TriggerSource::TRIGGER_KEY,
				50,
				0
			}) };
	trigParams.Texture = "green";
	trigParams.TextureRecording = "red";
	trigParams.TextureDitchDown = "blue";
	trigParams.TextureOverdubbing = "orange";
	trigParams.TexturePunchedIn = "purple";
	trigParams.DebounceMs = 120;
	station->AddTrigger(trigParams);
	_stations.push_back(station);

	_audioDevice = std::make_unique<AudioDevice>();

	for (auto& stat : _stations)
		stat->Init();
}

void Scene::Draw(DrawContext& ctx)
{
	// Draw scene
	auto &glCtx = dynamic_cast<GlDrawContext&>(ctx);
	glCtx.ClearMvp();
	glCtx.PushMvp(_viewProj);

	glCtx.PopMvp();

	// Draw overlays
	glCtx.ClearMvp();
	glCtx.PushMvp(_overlayViewProj);

	_label->Draw(ctx);

	for (auto& station : _stations)
		station->Draw(ctx);

	glCtx.PopMvp();
}

bool Scene::_InitResources(ResourceLib& resourceLib)
{
	_label->InitResources(resourceLib);

	for (auto& station : _stations)
		station->InitResources(resourceLib);

	InitSize();

	return Drawable::_InitResources(resourceLib);
}

bool Scene::_ReleaseResources()
{
	_label->ReleaseResources();

	for (auto& station : _stations)
		station->ReleaseResources();

	return Drawable::_ReleaseResources();
}

ActionResult Scene::OnAction(TouchAction action)
{
	action.SetActionTime(Timer::GetTime());
	std::cout << "Touch action " << action.Touch << " [" << action.State << "] " << action.Index << std::endl;

	if (TouchAction::TouchState::TOUCH_UP == action.State)
	{
		auto activeElement = _touchDownElement.lock();

		if (activeElement)
		{
			auto res = activeElement->OnAction(activeElement->GlobalToLocal(action));

			if (res.IsEaten)
			{
				if (nullptr != res.Undo)
					_undoHistory.Add(res.Undo);
			}
		}

		_touchDownElement.reset();

		return { false, ACTIONRESULT_DEFAULT };
	}

	for (auto& station : _stations)
	{
		auto res = station->OnAction(station->ParentToLocal(action));

		if (res.IsEaten)
		{
			if (nullptr != res.Undo)
				_undoHistory.Add(res.Undo);

			if (!_touchDownElement.lock())
				_touchDownElement = res.ActiveElement;

			return res;
		}
	}

	return { false, ACTIONRESULT_DEFAULT };
}

ActionResult Scene::OnAction(TouchMoveAction action)
{
	action.SetActionTime(Timer::GetTime());
	//std::cout << "Touch Move action " << action.Touch << " [" << action.Position.X << "," << action.Position.Y << "] " << action.Index << std::endl;
	
	auto activeElement = _touchDownElement.lock();

	if (activeElement)
		return activeElement->OnAction(activeElement->GlobalToLocal(action));

	return { false, ACTIONRESULT_DEFAULT };
}

ActionResult Scene::OnAction(KeyAction action)
{
	action.SetActionTime(Timer::GetTime());
	std::cout << "Key action " << action.KeyActionType << " [" << action.KeyChar << "] IsSytem:" << action.IsSystem << ", Modifiers:" << action.Modifiers << "]" << std::endl;

	if ((90 == action.KeyChar) && (actions::KeyAction::KEY_UP == action.KeyActionType) && (actions::MODIFIER_CTRL == action.Modifiers))
	{
		std::cout << ">> Undo <<" << std::endl;

		auto res = _undoHistory.Undo();

		return { res };
	}

	for (auto& station : _stations)
	{
		auto res = station->OnAction(action);

		if (res.IsEaten)
		{
			/*switch (res.ResultType)
			{
			case ACTIONRESULT_ID:
				_masterLoop = std::dynamic_pointer_cast<engine::Loop>(res.IdMasterLoop);
				break;
			}*/

			return res;
		}
	}

	return { false, ACTIONRESULT_DEFAULT };
}

void Scene::OnTick(Time curTime, unsigned int samps)
{
	for (auto& station : _stations)
	{
		station->OnTick(curTime, samps);
	}
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
		
		_channelMixer->SetParams(ChannelMixerParams({
				ChannelMixer::DefaultBufferSize,
				ChannelMixer::DefaultBufferSize,
				inParams.inputChannels,
				outParams.outputChannels}));

		_audioCallbackCount = 0;
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
		auto inDeviceInfo = nullptr == _audioDevice ?
			RtAudio::DeviceInfo() : _audioDevice->GetInputStreamInfo();
		_channelMixer->FromAdc(inBuf, inDeviceInfo.inputChannels, numSamps);

		for (auto& station : _stations)
		{
			_channelMixer->Source()->OnPlay(station, numSamps);
			station->EndMultiWrite(numSamps, true);
		}
	}

	_channelMixer->Source()->EndMultiPlay(numSamps);

	_channelMixer->Sink()->Zero(numSamps);

	if (nullptr != outBuf)
	{
		auto outDeviceInfo = nullptr == _audioDevice ?
			RtAudio::DeviceInfo() : _audioDevice->GetOutputStreamInfo();
		std::fill(outBuf, outBuf + numSamps * outDeviceInfo.outputChannels, 0.0f);

		for (auto& station : _stations)
		{
			station->OnPlay(_channelMixer->Sink(), numSamps);
			station->EndMultiPlay(numSamps);
		}

		_channelMixer->ToDac(outBuf, outDeviceInfo.outputChannels, numSamps);
	}
	else
	{
		for (auto& station : _stations)
		{
			station->OnPlay(_channelMixer->Sink(), numSamps);
			station->EndMultiPlay(numSamps);
		}
	}
	
	_channelMixer->Sink()->EndMultiWrite(numSamps, true);

	OnTick(Timer::GetTime(), numSamps);
}

bool Scene::OnUndo(std::shared_ptr<base::ActionUndo> undo)
{
	switch (undo->UndoType())
	{
	case UNDO_DOUBLE:
		auto doubleUndo = std::dynamic_pointer_cast<actions::DoubleActionUndo>(undo);
		if (doubleUndo)
		{
			doubleUndo->Value();
			return true;
		}		
	}

	return false;
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