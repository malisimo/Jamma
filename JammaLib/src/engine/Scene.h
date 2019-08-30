#pragma once
#include <memory>
#include <algorithm>
#include <mutex>
#include "../resources/ResourceLib.h"
#include "../audio/AudioDevice.h"
#include "../audio/ChannelMixer.h"
#include "../graphics/Image.h"
#include "../graphics/Camera.h"
#include "../graphics/GlDrawContext.h"
#include "../gui/GuiLabel.h"
#include "../gui/GuiSlider.h"
#include "Tickable.h"
#include "../io/JamFile.h"
#include "../io/RigFile.h"
#include "Drawable.h"
#include "ActionReceiver.h"
#include "AudioSource.h"
#include "Sizeable.h"
#include "GuiElement.h"
#include "Station.h"
#include "UndoHistory.h"

namespace engine
{
	class SceneParams :
		public base::DrawableParams,
		public base::SizeableParams
	{
	public:
		SceneParams(base::DrawableParams drawParams,
			base::SizeableParams sizeParams) :
			base::DrawableParams(drawParams),
			base::SizeableParams(sizeParams)
		{}
	};

	class Scene :
		public base::Tickable,
		public base::Drawable,
		public base::Sizeable,
		public base::ActionReceiver
	{
	public:
		Scene(SceneParams params);
		~Scene() { ReleaseResources(); }

		// Copy
		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;
		/*
		// Move
		Scene(Scene&& other) :
			base::Tickable(std::move(other)),
			base::Drawable(std::move(other)),
			base::Sizeable(std::move(other)),
			_viewProj(other._viewProj),
			_overlayViewProj(other._overlayViewProj),
			_channelMixer(std::move(other._channelMixer)),
			_audioDevice(std::move(other._audioDevice)),
			_label(std::move(other._label)),
			_undoHistory(std::move(other._undoHistory)),
			_stations(std::move(other._stations)),
			_touchDownElement(other._touchDownElement),
			_masterLoop(other._masterLoop)
		{
			other._stations = std::vector<std::shared_ptr<Station>>();
			other._viewProj = glm::mat4();
			other._overlayViewProj = glm::mat4();
			other._channelMixer = std::make_unique<audio::ChannelMixer>();
			other._audioDevice = std::make_unique<audio::AudioDevice>();
			other._label = std::make_unique<gui::GuiLabel>(
				gui::GuiLabelParams(
					base::GuiElementParams(
						base::DrawableParams{ "" },
						base::MoveableParams(utils::Position2d{ 0, 0 }, utils::Position3d{ 0, 0, 0 }, 1.0),
						base::SizeableParams{ 1,1 },
						"",
						"",
						"",
						{}),
					""));
			_undoHistory = UndoHistory();
			other._masterLoop = std::make_shared<Loop>(LoopParams());
		}

		Scene& operator=(Scene&& other)
		{
			if (this != &other)
			{
				ReleaseResources();

				std::swap(_viewProj, other._viewProj);
				std::swap(_overlayViewProj, other._overlayViewProj);
				_channelMixer.swap(other._channelMixer);
				_audioDevice.swap(other._audioDevice);
				_label.swap(other._label);
				_stations.swap(other._stations);
				_undoHistory.swap(other._undoHistory);
				std::swap(_touchDownElement, other._touchDownElement),
				_masterLoop.swap(other._masterLoop);
				std::swap(_drawParams, other._drawParams);
				std::swap(_sizeParams, other._sizeParams);
				std::swap(_texture, other._texture);
			}

			return *this;
		}*/

		static std::optional<std::shared_ptr<Scene>> FromFile(SceneParams sceneParams, io::JamFile jam, io::RigFile rig, std::wstring dir);
		
		virtual void Draw(base::DrawContext& ctx) override;
		virtual void Draw3d(base::DrawContext& ctx) override;

		virtual void SetSize(utils::Size2d size) override
		{
			_sizeParams.Size = size;
			InitSize();
		}
		unsigned int Width() const { return _sizeParams.Size.Width; }
		unsigned int Height() const	{ return _sizeParams.Size.Height; }

		virtual actions::ActionResult OnAction(actions::TouchAction action) override;
		virtual actions::ActionResult OnAction(actions::TouchMoveAction action) override;
		virtual actions::ActionResult OnAction(actions::KeyAction action) override;
		virtual void OnTick(Time curTime, unsigned int samps) override;

		void InitAudio();
		void CloseAudio();
		
	protected:
		virtual bool _InitResources(resources::ResourceLib& resourceLib) override;
		virtual bool _ReleaseResources() override;

		static int AudioCallback(void* outBuffer, void* inBuffer, unsigned int numSamps, double streamTime, RtAudioStreamStatus status, void* userData);
		void OnAudio(float* inBuffer, float* outBuffer, unsigned int numSamps);
		bool OnUndo(std::shared_ptr<base::ActionUndo> undo);
		void InitSize();
		glm::mat4 View();

		void AddStation(std::shared_ptr<Station> station);

	protected:
		static std::mutex _Mutex;

		bool _isSceneTouching;
		utils::Position2d _initTouchDownPosition;
		utils::Position3d _initTouchCamPosition;
		glm::mat4 _projection;
		glm::mat4 _view;
		glm::mat4 _overlayProjection;
		glm::mat4 _overlayView;
		std::shared_ptr<audio::ChannelMixer> _channelMixer;
		std::unique_ptr<audio::AudioDevice> _audioDevice;
		std::unique_ptr<gui::GuiLabel> _label;
		std::vector<std::shared_ptr<Station>> _stations;
		UndoHistory _undoHistory;
		std::weak_ptr<base::GuiElement> _touchDownElement;
		std::shared_ptr<Loop> _masterLoop;
		unsigned int _audioCallbackCount;
		graphics::Camera _camera;
	};
}
