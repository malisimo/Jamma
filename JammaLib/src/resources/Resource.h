#pragma once

#include <memory>
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

	template<typename T>
	static std::optional<std::weak_ptr<T>> ToResource(std::optional<std::weak_ptr<Resource>> resource)
	{
		if (!resource.has_value())
			return std::nullopt;

		auto res = resource.value().lock();

		if (!res)
			return std::nullopt;

		auto texRes = std::dynamic_pointer_cast<T>(res);

		if (texRes)
			return texRes;
		else
			return std::nullopt;
	}
};

