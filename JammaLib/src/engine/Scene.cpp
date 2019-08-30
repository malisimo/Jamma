#include "Scene.h"
#include "glm/ext.hpp"

using namespace base;
using namespace actions;
using namespace audio;
using namespace engine;
using namespace gui;
using namespace io;
using namespace graphics;
using namespace resources;
using namespace utils;

std::mutex Scene::_Mutex = std::mutex();

Scene::Scene(SceneParams params) :
	Drawable(params),
	Sizeable(params),
	_isSceneTouching(false),
	_projection(glm::mat4()),
	_view(glm::mat4()),
	_overlayProjection(glm::mat4()),
	_overlayView(glm::mat4()),
	_channelMixer(std::make_shared<ChannelMixer>(ChannelMixerParams{})),
	_label(std::unique_ptr<GuiLabel>()),
	_audioDevice(std::unique_ptr<AudioDevice>()),
	_masterLoop(std::shared_ptr<Loop>()),
	_stations(),
	_touchDownElement(std::weak_ptr<GuiElement>()),
	_audioCallbackCount(0),
	_camera(CameraParams(
		MoveableParams(
			Position2d{ 0,0 },
			Position3d{ 0, 0, 100 },
			1.0),
		0))
{
	GuiLabelParams labelParams(GuiElementParams(
		DrawableParams{ "" },
		MoveableParams(utils::Position2d{ 10, 10 }, utils::Position3d{ 10, 10, 0 }, 1.0),
		SizeableParams{ 200,80 },
		"",
		"",
		"",
		{}), "Jamma");
	_label = std::make_unique<GuiLabel>(labelParams);

	_audioDevice = std::make_unique<AudioDevice>();
}

std::optional<std::shared_ptr<Scene>> Scene::FromFile(SceneParams sceneParams, io::JamFile jamStruct, io::RigFile rigStruct, std::wstring dir)
{
	auto scene = std::make_shared<Scene>(sceneParams);

	TriggerParams trigParams;
	trigParams.Size = { 24, 24 };
	trigParams.Position = { 6, 6 };	
	trigParams.Texture = "green";
	trigParams.TextureRecording = "red";
	trigParams.TextureDitchDown = "blue";
	trigParams.TextureOverdubbing = "orange";
	trigParams.TexturePunchedIn = "purple";
	trigParams.DebounceMs = 120;

	StationParams stationParams;
	stationParams.Position = { 20, 20 };
	stationParams.ModelPosition = { -50, -20 };
	stationParams.Size = { 140, 80 };

	for (auto stationStruct : jamStruct.Stations)
	{
		auto station = Station::FromFile(stationParams, stationStruct, dir);
		if (station.has_value())
		{
			auto trigger = Trigger::FromFile(trigParams, rigStruct.Triggers[0]);

			if (trigger.has_value())
				station.value()->AddTrigger(trigger.value());

			scene->AddStation(station.value());
		}

		stationParams.Position += { 0, 90 };
		stationParams.ModelPosition += { 0, 90 };
	}

	return scene;
}

void Scene::Draw(DrawContext& ctx)
{
	glDisable(GL_DEPTH_TEST);

	// Draw overlays
	auto &glCtx = dynamic_cast<GlDrawContext&>(ctx);
	glCtx.ClearModelView();
	glCtx.SetProjection(_overlayProjection);
	glCtx.PushModelView(_overlayView);

	_label->Draw(ctx);

	for (auto& station : _stations)
		station->Draw(ctx);

	glCtx.PopModelView();
}

void Scene::Draw3d(DrawContext& ctx)
{
	glEnable(GL_DEPTH_TEST);

	// Draw scene
	auto& glCtx = dynamic_cast<GlDrawContext&>(ctx);
	glCtx.ClearModelView();
	glCtx.SetProjection(_projection);
	glCtx.PushModelView(_view);

	for (auto& station : _stations)
		station->Draw3d(ctx);

	glCtx.PopModelView();
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
		else if (_isSceneTouching)
			_isSceneTouching = false;

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
	
	_isSceneTouching = true;
	_initTouchDownPosition = action.Position;
	_initTouchCamPosition = _camera.ModelPosition();

	ActionResult res;
	res.IsEaten = true;
	res.Id = 0;
	res.ResultType = ACTIONRESULT_ID;
	res.Undo = std::shared_ptr<ActionUndo>();
	res.ActiveElement = std::weak_ptr<GuiElement>();
	return res;
}

ActionResult Scene::OnAction(TouchMoveAction action)
{
	action.SetActionTime(Timer::GetTime());
	
	auto activeElement = _touchDownElement.lock();

	if (activeElement)
		return activeElement->OnAction(activeElement->GlobalToLocal(action));
	else if (_isSceneTouching)
	{
		auto dPos = action.Position - _initTouchDownPosition;
		_camera.SetModelPosition(_initTouchCamPosition - Position3d{ (float)dPos.X, (float)dPos.Y, 0.0 });
		SetSize(_sizeParams.Size);
	}

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
	std::unique_lock<std::mutex> lck(_Mutex, std::defer_lock);
	lck.lock();

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

	lck.unlock();
}

void Scene::CloseAudio()
{
	std::unique_lock<std::mutex> lck(_Mutex, std::defer_lock);
	lck.lock();

	_audioDevice->Stop();

	lck.unlock();
}

int Scene::AudioCallback(void* outBuffer,
	void* inBuffer,
	unsigned int numSamps,
	double streamTime,
	RtAudioStreamStatus status,
	void* userData)
{
	std::unique_lock<std::mutex> lck(_Mutex, std::defer_lock);
	lck.lock();

	Scene* scene = (Scene*)userData;
	scene->OnAudio((float*)inBuffer, (float*)outBuffer, numSamps);

	lck.unlock();

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
	_projection = glm::perspective(glm::radians(60.0f), ar, 0.5f, 300.0f);
	_view = View();

	auto hScale = _sizeParams.Size.Width > 0 ? 2.0f / (float)_sizeParams.Size.Width : 1.0f;
	auto vScale = _sizeParams.Size.Height > 0 ? 2.0f / (float)_sizeParams.Size.Height : 1.0f;
	_overlayProjection = glm::mat4(1.0);
	_overlayView = glm::translate(glm::mat4(1.0), glm::vec3(-1.0f, -1.0f, -1.0f));
	_overlayView = glm::scale(glm::mat4(1.0), glm::vec3(hScale, vScale, 1.0f));
}

glm::mat4 Scene::View()
{
	auto camPos = _camera.ModelPosition();
	return glm::lookAt(glm::vec3(camPos.X, camPos.Y, camPos.Z), glm::vec3(camPos.X, camPos.Y, 0.f), glm::vec3(0.f, 1.f, 0.f));
}

void Scene::AddStation(std::shared_ptr<Station> station)
{
	_stations.push_back(station);
	station->Init();
}
