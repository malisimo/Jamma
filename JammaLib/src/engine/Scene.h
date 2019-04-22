#pragma once
#include <memory>
#include <algorithm>
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "../resources/ResourceLib.h"
#include "../audio/AudioDevice.h"
#include "../audio/ChannelMixer.h"
#include "../graphics/Image.h"
#include "../graphics/GlDrawContext.h"
#include "../gui/GuiLabel.h"
#include "../gui/GuiSlider.h"
#include "Drawable.h"
#include "ActionReceiver.h"
#include "Audible.h"
#include "Sizeable.h"
#include "Loop.h"

namespace engine
{
	class SceneParams :
		public base::DrawableParams,
		public base::SizeableParams,
		public base::AudibleParams
	{
	public:
		SceneParams(base::DrawableParams drawParams,
			base::SizeableParams sizeParams,
			base::AudibleParams audibleParams) :
			base::DrawableParams(drawParams),
			base::SizeableParams(sizeParams),
			base::AudibleParams(audibleParams)
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
			_audioMixer(std::move(other._audioMixer)),
			_audioDevice(std::move(other._audioDevice)),
			_loop(std::move(other._loop)),
			_image(std::move(other._image)),
			_label(std::move(other._label)),
			_slider(std::move(other._slider))
		{
			other._audioDevice = std::make_unique<audio::AudioDevice>();
			other._loop = std::make_unique<Loop>(LoopParams());
			other._image = std::make_unique<graphics::Image>(
				graphics::ImageParams(
					base::DrawableParams{ "" },
					base::SizeableParams{ 1, 1 },
					"texture"));
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
			other._slider = std::make_unique<gui::GuiSlider>(gui::GuiSliderParams());
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
				_slider.swap(other._slider);
				_image.swap(other._image);
				_loop.swap(other._loop);
				_audioDevice.swap(other._audioDevice);
			}

			return *this;
		}

		virtual void Draw(base::DrawContext& ctx) override;
		virtual bool InitResources(resources::ResourceLib& resourceLib) override;
		virtual bool ReleaseResources() override;

		virtual void SetSize(utils::Size2d size) override;
		unsigned int Width() const;
		unsigned int Height() const;

		virtual void OnAction(actions::TouchAction touchAction) override;
		virtual void OnAction(actions::TouchMoveAction touchAction) override;
		virtual void OnAction(actions::KeyAction keyAction) override;

		void InitAudio();
		audio::ChannelMixer& GetMixer();
		RtAudio::DeviceInfo AudioInputDeviceInfo();
		RtAudio::DeviceInfo AudioOutputDeviceInfo();

	private:
		static int OnAudio(void* outBuffer, void* inBuffer, unsigned int numSamps, double streamTime, RtAudioStreamStatus status, void* userData);
		void InitSize();
		glm::mat4 View();

	private:
		glm::mat4 _viewProj;
		glm::mat4 _overlayViewProj;
		audio::ChannelMixer _audioMixer;
		std::unique_ptr<audio::AudioDevice> _audioDevice;
		std::unique_ptr<gui::GuiLabel> _label;
		std::unique_ptr<gui::GuiSlider> _slider;
		std::unique_ptr<graphics::Image> _image;
		std::unique_ptr<Loop> _loop;
	};
}
