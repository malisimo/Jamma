#pragma once

#include <string>
#include <iostream>
#include <optional>
#include <gl/glew.h>
#include <gl/gl.h>
#include "glm/glm.hpp"
#include "../graphics/Shader.h"
#include "Resource.h"

class ShaderResource : public Resource
{
public:
	ShaderResource(std::string name, GLuint shaderProgram, std::vector<std::string> uniforms);
	~ShaderResource();

	// Delete the copy constructor/assignment
	ShaderResource(const ShaderResource &) = delete;
	ShaderResource& operator=(const ShaderResource &) = delete;

	ShaderResource(ShaderResource &&other) :
		_name(other._name),
		_shaderProgram(other._shaderProgram),
		_uniforms(other._uniforms)
	{
		other._name = "";
		other._shaderProgram = 0;
		other._uniforms = {};

		std::cout << "Moving ShaderResource" << std::endl;
	}

	ShaderResource& operator=(ShaderResource &&other)
	{
		if (this != &other)
		{
			std::cout << "Swapping ShaderResource" << std::endl;

			Release();
			std::swap(_name, other._name);
			std::swap(_shaderProgram, other._shaderProgram);
			std::swap(_uniforms, other._uniforms);
		}

		return *this;
	}

	virtual Resources::Type GetType() const override { return Resources::SHADER; }
	virtual GLuint GetId() const override { return _shaderProgram; }
	virtual void Release() override;
	
	void SetUniforms(GlDrawContext& ctx);

	static std::optional<GLuint> Load(const std::string& vertFilePath, const std::string& fragFilePath);

private:
	void InitUniforms(std::vector<std::string> uniforms);

	static bool AddStageFromFile(GLuint shaderProgram, const std::string& filePath, GLenum shaderType);

private:
	std::string _name;
	GLuint _shaderProgram;
	std::map<std::string, GLint> _uniforms;
};
