#include "GlUtils.h"


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