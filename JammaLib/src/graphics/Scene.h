#pragma once
#include "Drawable.h"
#include "ActionReceiver.h"
#include "Audible.h"
#include "GlDrawContext.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

class Scene : 
	public Drawable,
	public Audible,
	public ActionReceiver
{
public:
	Scene();
	~Scene();

	void Init();
	void Draw(const DrawContext& ctx);

private:
	glm::mat4 View();

private:
	glm::mat4 MVP;
	int _width;
	int _height;
};

