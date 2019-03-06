#pragma once
#include <memory>
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "Drawable.h"
#include "../resources/ResourceLib.h"
#include "../audio/AudioDevice.h"
#include "../engine/Loop.h"
#include "Image.h"
#include "../gui/GuiLabel.h"
#include "../gui/GuiSlider.h"
#include "ActionReceiver.h"
#include "Audible.h"
#include "Sizeable.h"
#include "GlDrawContext.h"

class SceneParams :
	public DrawableParams,
	public SizeableParams,
	public AudibleParams
{
public:
	SceneParams(DrawableParams drawParams,
		SizeableParams sizeParams,
		AudibleParams audibleParams) :
		DrawableParams(drawParams),
		SizeableParams(sizeParams),
		AudibleParams(audibleParams)
	{}
};

class Scene :
	public std::enable_shared_from_this<Scene>,
	public Drawable,
	public Sizeable,
	public Audible,
	public ActionReceiver
{
public:
	Scene(SceneParams params);
	~Scene() { ReleaseResources(); }

	// Copy
	Scene(const Scene &) = delete;
	Scene& operator=(const Scene &) = delete;

	// Move
	Scene(Scene &&other) :
		Drawable(std::move(other)),
		Sizeable(std::move(other)),
		Audible(std::move(other)),
		_viewProj(other._viewProj),
		_overlayViewProj(other._overlayViewProj),
		_audioDevice(std::move(other._audioDevice)),
		_loop(std::move(other._loop)),
		_image(std::move(other._image)),
		_label(std::move(other._label)),
		_slider(std::move(other._slider))
	{
		other._audioDevice = std::make_unique<AudioDevice>();
		other._loop = std::make_unique<Loop>(LoopParams());
		other._image = std::make_unique<Image>(ImageParams(DrawableParams{ "" }, SizeableParams{ 1, 1 }, "texture"));
		other._label = std::make_unique<GuiLabel>(GuiLabelParams(GuiElementParams(DrawableParams{ "" }, MoveableParams{ 0,0 }, SizeableParams{ 1,1 }, "", "", "", {}), ""));
		other._slider = std::make_unique<GuiSlider>(GuiSliderParams());
		other._viewProj = glm::mat4();
		other._overlayViewProj = glm::mat4();
	}

	Scene& operator=(Scene &&other)
	{
		if (this != &other)
		{
			ReleaseResources();
			std::swap(_drawParams, other._drawParams),
			std::swap(_sizeParams, other._sizeParams),
			std::swap(_audibleParams, other._audibleParams),
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

	virtual void Draw(DrawContext& ctx) override;
	virtual bool InitResources(ResourceLib& resourceLib) override;
	virtual bool ReleaseResources() override;

	virtual void SetSize(Size2d size) override;
	unsigned int Width() const;
	unsigned int Height() const;

	virtual void OnAction(TouchAction touchAction) override;
	virtual void OnAction(TouchMoveAction touchAction) override;
	virtual void OnAction(KeyAction keyAction) override;

	virtual void Play(float* buf, unsigned int numChans, unsigned int numSamps) override;

	void InitAudio();
	RtAudio::DeviceInfo AudioInputDeviceInfo();
	RtAudio::DeviceInfo AudioOutputDeviceInfo();

private:
	static int OnAudio(void* outBuffer, void* inBuffer, unsigned int numSamps, double streamTime, RtAudioStreamStatus status, void* userData);
	void InitSize();
	glm::mat4 View();

private:
	glm::mat4 _viewProj;
	glm::mat4 _overlayViewProj;
	std::unique_ptr<GuiLabel> _label;
	std::unique_ptr<GuiSlider> _slider;
	std::unique_ptr<Image> _image;
	std::unique_ptr<Loop> _loop;
	std::unique_ptr<AudioDevice> _audioDevice;
};
