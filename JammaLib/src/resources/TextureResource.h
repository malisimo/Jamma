#pragma once

#include <string>
#include <iostream>
#include <optional>
#include <gl/glew.h>
#include <gl/gl.h>
#include "Resource.h"
#include "../utils/ImageUtils.h"

namespace resources
{
	class TextureResource : public Resource
	{
	public:
		TextureResource(std::string name, GLuint texture, unsigned int width, unsigned int height);
		~TextureResource();

		// Delete the copy constructor/assignment
		TextureResource(const TextureResource&) = delete;
		TextureResource& operator=(const TextureResource&) = delete;

		TextureResource(TextureResource&& other) :
			Resource(other._name),
			_width(other._width),
			_height(other._height),
			_texture(other._texture)
		{
			std::cout << "Moving TextureResource" << std::endl;

			other._name = "";
			other._width = 0;
			other._height = 0;
			other._texture = 0;
		}

		TextureResource& operator=(TextureResource&& other)
		{
			if (this != &other)
			{
				std::cout << "Swapping TextureResource" << std::endl;

				Release();
				std::swap(_name, other._name);
				std::swap(_width, other._width);
				std::swap(_height, other._height);
				std::swap(_texture, other._texture);
			}

			return *this;
		}

		virtual Type GetType() const override { return TEXTURE; }
		virtual GLuint GetId() const override { return _texture; }
		virtual void Release() override;

		unsigned int Width() { return _width; }
		unsigned int Height() { return _height; }

		static std::optional<std::tuple<GLuint, unsigned int, unsigned int>> Load(std::string fileName);

	private:
		unsigned int _width;
		unsigned int _height;
		GLuint _texture;
	};
}
