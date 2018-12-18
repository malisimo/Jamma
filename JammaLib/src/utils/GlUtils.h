#pragma once

#include <string>
#include <iostream>
#include <gl/glew.h>
#include <gl/gl.h>
#include "gl/glext.h"
#include "gl/wglext.h"
#include "glm/glm.hpp"

class GlUtils
{
public:
	static bool CheckError(const std::string& log);
};

