#pragma once
#include <memory>
#include <algorithm>
#include "../resources/ResourceLib.h"
#include "../audio/AudioDevice.h"
#include "../audio/ChannelMixer.h"
#include "../graphics/Image.h"
#include "../graphics/GlDrawContext.h"
#include "../gui/GuiLabel.h"
#include "../gui/GuiSlider.h"
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
		public base::SizeableParams,
		public base::AudioSourceParams
	{
	public:
		SceneParams(base::DrawableParams drawParams,
			base::SizeableParams sizeParams,
			base::AudioSourceParams AudioSourceParams) :
			base::DrawableParams(drawParams),
			base::SizeableParams(sizeParams),
			base::AudioSourceParams(AudioSourceParams)
		{}
	};

	class Scene :
		public std::enable_shared_from_this<Scene>,
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

		// Move
		Scene(Scene&& other) :
			base::Drawable(std::move(other)),
			base::Sizeable(std::move(other)),
			_viewProj(other._viewProj),
			_overlayViewProj(other._overlayViewProj),
			_channelMixer(std::move(other._channelMixer)),
			_audioDevice(std::move(other._audioDevice)),
			_stations(std::move(other._stations)),
			_label(std::move(other._label))
		{
			other._audioDevice = std::make_unique<audio::AudioDevice>();
			other._stations = std::vector<std::shared_ptr<Station>>();
			other._label = std::make_unique<gui::GuiLabel>(
				gui::GuiLabelParams(
					base::GuiElementParams(
						base::DrawableParams{ "" },
						base::MoveableParams{ 0,0 },
						base::SizeableParams{ 1,1 },
						"",
						"",
						"",
						{}),
					""));
			other._viewProj = glm::mat4();
			other._overlayViewProj = glm::mat4();
		}

		Scene& operator=(Scene&& other)
		{
			if (this != &other)
			{
				ReleaseResources();

				std::swap(_drawParams, other._drawParams),
				std::swap(_sizeParams, other._sizeParams),
				std::swap(_texture, other._texture),
				std::swap(_resizing, other._resizing),
				std::swap(_viewProj, other._viewProj);
				std::swap(_overlayViewProj, other._overlayViewProj);
				_label.swap(other._label);
				_stations.swap(other._stations);
				_audioDevice.swap(other._audioDevice);
			}

			return *this;
		}

		virtual void Draw(base::DrawContext& ctx) override;

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

		void InitAudio();

	protected:
		virtual bool _InitResources(resources::ResourceLib& resourceLib) override;
		virtual bool _ReleaseResources() override;

	private:
		static int AudioCallback(void* outBuffer, void* inBuffer, unsigned int numSamps, double streamTime, RtAudioStreamStatus status, void* userData);
		void OnAudio(float* inBuffer, float* outBuffer, unsigned int numSamps);
		bool OnUndo(std::shared_ptr<base::ActionUndo> undo);
		void InitSize();
		glm::mat4 View();

	private:
		glm::mat4 _viewProj;
		glm::mat4 _overlayViewProj;
		audio::ChannelMixer _channelMixer;
		std::unique_ptr<audio::AudioDevice> _audioDevice;
		std::unique_ptr<gui::GuiLabel> _label;
		std::vector<std::shared_ptr<Station>> _stations;
		UndoHistory _undoHistory;
		std::weak_ptr<base::GuiElement> _touchDownElement;
	};
}
