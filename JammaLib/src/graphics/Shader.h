#pragma once

#include <optional>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include "GlUtils.h"
#include "../resources/Resource.h"
#include "GlDrawContext.h"

namespace graphics
{
	class Shader
	{
	public:
		Shader();
		Shader(GLuint shader);
		~Shader();

		static std::optional<Shader> FromFile(const std::string& filePath, GLenum shaderType);

		bool Init(GLuint program, std::vector<std::string> uniforms);
		void Activate(GlDrawContext& ctx);
		void Release();
		GLuint Name() const;

	private:
		enum ShaderState { EMPTY, COMPILED, READY };

		ShaderState _state;
		GLuint _shader;

		std::map<std::string, GLint> _uniforms;
	};
}
