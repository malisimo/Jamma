#pragma once
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "Drawable.h"
#include "../resources/ResourceLib.h"
#include "Image.h"
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
	~Scene() { Destroy(); }

	// Copy
	Scene(const Scene &) = delete;
	Scene& operator=(const Scene &) = delete;

	// Move
	Scene(Scene &&other) :
		_width(other._width),
		_height(other._height),
		_mvp(other._mvp),
		_image(std::move(other._image))
	{
		other._image = std::make_unique<Image>();
		other._mvp = glm::mat4();
	}

	Scene& operator=(Scene &&other)
	{
		if (this != &other)
		{
			Destroy();
			std::swap(_mvp, other._mvp);
			_image.swap(other._image);
		}

		return *this;
	}

	virtual void Init(ResourceLib& resourceLib);
	virtual void Draw(DrawContext& ctx);
	virtual bool Destroy();

	int Width() const;
	int Height() const;

private:
	glm::mat4 View();

private:
	int _width;
	int _height;
	glm::mat4 _mvp;
	std::unique_ptr<Image> _image;
};
