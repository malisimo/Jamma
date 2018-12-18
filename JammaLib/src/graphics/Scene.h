#pragma once
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "Drawable.h"
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

	// Delete the copy constructor/assignment
	Scene(const Scene &) = delete;
	Scene &operator=(const Scene &) = delete;

	Scene(Scene &&other) :
		_mvp(other._mvp),
		_image(std::move(other._image))
	{
		other._image = Image();
		other._mvp = glm::mat4();
	}

	Scene &operator=(Scene &&other)
	{
		if (this != &other)
		{
			Destroy();
			std::swap(_mvp, other._mvp);
			std::swap(_image, other._image);
		}
	}

	virtual void Init();
	virtual void Draw(const DrawContext& ctx);
	virtual bool Destroy();

	int Width() const;
	int Height() const;

private:
	glm::mat4 View();

private:
	glm::mat4 _mvp;
	int _width;
	int _height;

	Image _image;
};
