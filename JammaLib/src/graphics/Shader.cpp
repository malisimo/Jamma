#include "Shader.h"


Shader::Shader() :
	_state(EMPTY),
	_shader(0)
{
}

Shader::Shader(GLuint shader) :
	_state(READY),
	_shader(shader)
{
	Init();
}

Shader::~Shader()
{
}

bool Shader::Init()
{
	/*GLuint shader = glCreateShader(shaderType);
	if (shader == 0)
		throw std::runtime_error("glCreateShader failed");

	const char* code = buffer.str().c_str();
	glShaderSource(shader, 1, (const GLchar**)&code, NULL);
	glCompileShader(shader);*/

	return true;
}

void Shader::Activate(const GlDrawContext& ctx)
{
	if (_state == READY)
	{
		glUseProgram(_shader);
		glUniformMatrix4fv(_uniformMvp, 1, GL_FALSE, &ctx.GetMvp()[0][0]);
	}
}

void Shader::Destroy()
{
	glDeleteShader(_shader);

	_state = EMPTY;
}

std::optional<Shader> Shader::FromFile(const std::string& filePath, GLenum shaderType)
{
	std::ifstream f;
	f.open(filePath.c_str(), std::ios::in | std::ios::binary);

	if (!f.is_open()) {
		throw std::runtime_error(std::string("Failed to open file: ") + filePath);
	}

	std::stringstream buffer;
	buffer << f.rdbuf();

	GLuint shader = glCreateShader(shaderType);
	if (shader == 0)
	{
		std::cout << "glCreateShader failed" << std::endl;
		//throw std::runtime_error("glCreateShader failed");
		return std::nullopt;
	}

	auto shaderStr = buffer.str();
	const char* code = shaderStr.c_str();
	glShaderSource(shader, 1, (const GLchar**)&code, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		std::string msg("Compile failure in shader:\n");

		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		auto strInfoLog = new char[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
		msg += strInfoLog;
		delete[] strInfoLog;

		glDeleteShader(shader); shader = 0;

		std::cout << msg << std::endl;

		return std::optional<Shader>();
	}

	return std::optional<Shader>(Shader(shader));
}