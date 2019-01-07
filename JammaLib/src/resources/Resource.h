#pragma once

#include <gl/glew.h>
#include <gl/gl.h>

namespace Resources
{
	enum Type
	{
		NONE = 0,
		TEXTURE = 1,
		SHADER = 2
	};
};

class Resource
{
public:
	virtual Resources::Type GetType() const { return Resources::NONE; }
	virtual GLuint GetId() const { return 0; }
	virtual void Release() {}
};

