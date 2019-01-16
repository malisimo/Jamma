#pragma once
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "Drawable.h"
#include "../resources/ResourceLib.h"
#include "Image.h"
#include "../gui/GuiLabel.h"
#include "ActionReceiver.h"
#include "Audible.h"
#include "GlDrawContext.h"

class Scene : 
	public Drawable,
	public Audible,
	public ActionReceiver
{
public:
	Scene(int width, int height);
	~Scene() { Release(); }

	// Copy
	Scene(const Scene &) = delete;
	Scene& operator=(const Scene &) = delete;

	// Move
	Scene(Scene &&other) :
		_width(other._width),
		_height(other._height),
		_viewProj(other._viewProj),
		_overlayViewProj(other._overlayViewProj),
		_image(std::move(other._image))
	{
		other._image = std::make_unique<Image>();
		other._viewProj = glm::mat4();
		other._overlayViewProj = glm::mat4();
	}

	Scene& operator=(Scene &&other)
	{
		if (this != &other)
		{
			Release();
			std::swap(_viewProj, other._viewProj);
			std::swap(_overlayViewProj, other._overlayViewProj);
			_image.swap(other._image);
		}

		return *this;
	}

	virtual void Init(ResourceLib& resourceLib);
	virtual void Draw(DrawContext& ctx);
	virtual bool Release();

	int Width() const;
	int Height() const;

private:
	glm::mat4 View();

private:
	int _width;
	int _height;
	glm::mat4 _viewProj;
	glm::mat4 _overlayViewProj;
	std::unique_ptr<GuiLabel> _label;
	std::unique_ptr<Image> _image;
};
