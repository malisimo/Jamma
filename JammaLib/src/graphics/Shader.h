#pragma once

#include <optional>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <gl/glew.h>
#include <gl/gl.h>
#include "gl/glext.h"
#include "gl/wglext.h"
#include "glm/glm.hpp"
#include "GlDrawContext.h"


class Shader
{
public:
	Shader();
	Shader(GLuint shader);
	~Shader();

	static std::optional<Shader> FromFile(const std::string& filePath, GLenum shaderType);

	bool Init();
	void Activate(const GlDrawContext& ctx);
	void Destroy();

private:
	enum ShaderState { EMPTY, COMPILED, READY };

private:
	ShaderState _state;
	GLuint _shader;
	GLuint _uniformMvp;
};

