#include "GlUtils.h"
#include <iostream>
#include <gl/glew.h>
#include <gl/gl.h>
#include "gl/glext.h"
#include "gl/wglext.h"
#include "glm/glm.hpp"

using namespace utils;

bool GlUtils::CheckError(const std::string& log)
{
	bool foundError = false;

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		auto errStr = glewGetErrorString(err);
		foundError = true;
		std::cout << errStr << std::endl;
	}

	if (foundError)
		std::cout << log << std::endl;

	return !foundError;
}