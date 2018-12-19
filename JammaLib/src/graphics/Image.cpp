#include "Image.h"

Image::Image() :
	_width(1024),
	_height(768),
	_texture(0),
	_textureName(0),
	_vertexArray(0),
	_vertexBuffer{ 0,0 },
	_shaderProgram(0)
{
}

bool Image::Init()
{
	bool Validated = true;

	if (Validated)
		Validated = InitShader();
	if (Validated)
		Validated = InitTexture();
	if (Validated)
		Validated = InitVertexArray();

	return Validated && GlUtils::CheckError("Image::Init()");
}

void Image::Draw(DrawContext& ctx)
{
	glUseProgram(_shaderProgram);

	_vertShader.Activate(dynamic_cast<GlDrawContext&>(ctx));
	_fragShader.Activate(dynamic_cast<GlDrawContext&>(ctx));

	glBindVertexArray(_vertexArray);
	//glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer[0]);
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);

	//glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 1);
	glDrawArrays(GL_TRIANGLES, 0, VertexCount);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

bool Image::Destroy()
{
	_vertShader.Destroy();
	_vertShader = Shader();
	_fragShader.Destroy();
	_fragShader = Shader();

	glDeleteTextures(1, &_texture);
	_texture = 0;
	_textureName = 0;

	glDeleteBuffers(2, _vertexBuffer);
	_vertexBuffer[0] = 0;
	_vertexBuffer[1] = 0;

	glDeleteVertexArrays(1, &_vertexArray);
	_vertexArray = 0;

	return true;
}

bool Image::InitShader()
{
	auto res = true;

	_shaderProgram = glCreateProgram();
		
	{
		auto shader = Shader::FromFile("./shaders/texture.vert", GL_VERTEX_SHADER);
		if (shader.has_value())
		{
			_vertShader = shader.value();
			glAttachShader(_shaderProgram, _vertShader.Name());
		}
		else
		{
			res = false;
			_vertShader = Shader();
		}
	}

	{
		auto shader = Shader::FromFile("./shaders/texture.frag", GL_FRAGMENT_SHADER);
		if (shader.has_value())
		{
			_fragShader = shader.value();
			glAttachShader(_shaderProgram, _fragShader.Name());
		}
		else
		{
			res = false;
			_fragShader = Shader();
		}
	}

	if (res)
	{
		glLinkProgram(_shaderProgram);

		GLint glResult;
		glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &glResult);

		glUseProgram(_shaderProgram);

		_vertShader.Init(_shaderProgram, { "MVP" });
		_fragShader.Init(_shaderProgram, {});

		glUseProgram(0);
	}

	return res;
}

bool Image::InitTexture()
{
	auto imageLoaded = LoadTga("./textures/grid.tga");

	if (!imageLoaded.has_value())
		return false;

	auto imageTuple = imageLoaded.value();

	auto pixels = std::get<0>(imageTuple);
	auto width = std::get<1>(imageTuple);
	auto height = std::get<2>(imageTuple);

	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels.data());

	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

bool Image::InitVertexArray()
{
	glGenVertexArrays(1, &_vertexArray);
	glBindVertexArray(_vertexArray);

	glGenBuffers(2, _vertexBuffer);

	static const GLfloat verts[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.5f,  0.5f, 0.0f,
	};

	glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	static const GLfloat uvs[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
	};

	glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), uvs, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	GlUtils::CheckError("Image::InitVertexArray");

	return true;
}

std::optional<std::tuple<std::vector<unsigned char>, int, int>> Image::LoadTga(const std::string& fileName)
{
	FILE* file;
	auto res = fopen_s(&file, fileName.c_str(), "rb");

	if (!file)
		return std::nullopt;

	// Read the header of the TGA, compare it with the known headers for compressed and uncompressed TGAs
	unsigned char header[18];
	fread(header, sizeof(unsigned char) * 18, 1, file);

	while (header[0] > 0)
	{
		--header[0];

		unsigned char temp;
		fread(&temp, sizeof(unsigned char), 1, file);
	}

	auto width = header[13] * 256 + header[12];
	auto height = header[15] * 256 + header[14];
	auto bpp = header[16] / 8;

	// check whether width, height an BitsPerPixel are valid
	if ((width <= 0) || (height <= 0) || ((bpp != 1) && (bpp != 3) && (bpp != 4)))
	{
		fclose(file);
		return std::nullopt;
	}

	// allocate the TgaLoader-buffer
	auto pixels = std::vector<unsigned char>(width * height * 4);

	if (header[2] == 2)
	{
		unsigned char pixel[4] = { 255, 255, 255, 255 };
		const int numPixels = width * height;

		for (int i = 0; i < numPixels; ++i)
		{
			fread(pixel, sizeof(unsigned char) * bpp, 1, file);

			if (bpp == 1)
			{
				pixel[1] = pixel[0];
				pixel[2] = pixel[0];
				pixel[3] = pixel[0];
			}

			pixels[(i * 4) + 0] = pixel[0];
			pixels[(i * 4) + 1] = pixel[1];
			pixels[(i * 4) + 2] = pixel[2];
			pixels[(i * 4) + 3] = pixel[3];
		}
	}
	else
	{
		fclose(file);
		return std::nullopt;
	}

	fclose(file);
	return std::tuple<std::vector<unsigned char>, int, int>(pixels, width, height);
}

