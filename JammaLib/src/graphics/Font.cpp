#include "Font.h"

// Font class represents an individual font of a particular size

Font::Font()
{
}

Font::Font(FontOptions::FontParams params,
	std::vector<float> charWidths,
	std::weak_ptr<TextureResource> texture,
	std::weak_ptr<ShaderResource> shader) :
	_params(params),
	_charWidths(charWidths),
	_texture(texture),
	_shader(shader)
{
}

std::optional<std::unique_ptr<Font>> Font::Load(FontOptions::FontSize size,
	std::weak_ptr<TextureResource> texture,
	std::weak_ptr<ShaderResource> shader)
{
	auto datafilename = GetFontFilename(size);

	std::ifstream inputFile;
	inputFile.open(datafilename.c_str());

	if (!inputFile)
	{
		std::cout << "Error opening file " << datafilename << "!\n";

		inputFile.close();

		return std::nullopt;
	}

	// Declare a tempory holder for the lines to be read (of type string)
	int linenum = 0;
	unsigned int param1;
	float param2;
	unsigned int gridSize = 1;
	unsigned int spaceChar = 0;
	unsigned int degreeChar = 0;
	float charHeight = 16.f;
	std::vector<float> charWidths;

	std::string line;
	unsigned int texWidth = 0;
	unsigned int texHeight = 0;

	auto textureResource = texture.lock();
	if (textureResource)
	{
		texWidth = textureResource->Width();
		texHeight = textureResource->Height();
	}

	// While we are able to get lines from the file
	while (std::getline(inputFile, line))
	{
		linenum++;

		if (linenum > 4)
			break;

		// Parse the line from the file and
		// set appropriate parameters
		std::stringstream ss(line);

		switch (linenum)
		{
		case 1:
			while (ss >> param2)
			{
				charWidths.push_back(param2 + 1.0f);
			}
			break;
		case 2:
			if (ss >> param2)
				charHeight = param2;

			break;
		case 3:
			if (ss >> param1)
				gridSize = param1;

			break;
		case 4:
			if (ss >> param1)
				spaceChar = param1;

			break;
		case 5:
			if (ss >> param1)
				degreeChar = param1;

			break;
		}
	}

	auto numWidth = (unsigned int)(((double)texWidth) / ((double)gridSize));
	auto numHeight = (unsigned int)(((double)texHeight) / ((double)gridSize));

	auto numChars = charWidths.size();
	if (numChars > 0)
	{
		float totalwidth = 0.0f;

		for (auto width : charWidths)
		{
			totalwidth += width;
		}

		if (spaceChar < numChars)
			charWidths[spaceChar] = (totalwidth / ((float)numChars)) - 4.0f;

		FontOptions::FontParams fontParams = { numWidth, numHeight, gridSize, charHeight, spaceChar, degreeChar, size };
		return std::make_unique<Font>(fontParams, charWidths, texture, shader);
	}

	return std::nullopt;
}

std::string Font::GetFontName(FontOptions::FontSize size)
{
	std::string fontname("");

	switch (size)
	{
	case FontOptions::FONT_TINY:
		return "font_tiny";
	case FontOptions::FONT_SMALL:
		return "font_small";
	case FontOptions::FONT_MEDIUM:
		return "font_medium";
	case FontOptions::FONT_LARGE:
		return "font_large";
	}

	return "";
}

GLuint Font::InitVertexArray(const std::string& str, GLenum usage)
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);

	auto numChars = str.size();
	auto numVerts = numChars * 6;
	std::vector<GLfloat> posUv(numVerts * 5);

	for (unsigned int i = 0; i < numChars; i++)
	{
		FillPosUv(posUv, i, str[i]);
	}

	static const GLfloat verts[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.5f,  0.5f, 0.0f,
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, posUv.size() * sizeof(GLfloat), posUv.data(), usage);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	GlUtils::CheckError("Font::InitVertexArray");

	return vao;
}

void Font::Draw(GlDrawContext& ctx, GLuint vertexArray, unsigned int numChars)
{
	auto texture = _texture.lock();
	auto shader = _shader.lock();

	if (!texture || !shader)
		return;

	glUseProgram(shader->GetId());
	shader->SetUniforms(ctx);

	glBindVertexArray(vertexArray);

	glBindTexture(GL_TEXTURE_2D, texture->GetId());

	//glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 1);
	glDrawArrays(GL_TRIANGLES, 0, numChars * 6);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

float Font::MeasureString(const std::string& str)
{
	const char* chars = str.c_str();
	unsigned int len = (unsigned int)strlen(chars);

	float totallength = 0.0f;
	int charnum;

	for (unsigned int counter1 = 0; counter1<len; counter1++)
	{
		charnum = GetCharNum(chars[counter1]);

		if (charnum >= 0)
			totallength += (float)((int)(_charWidths[charnum] + 0.5f));
	}

	return totallength;
}

float Font::GetHeight()
{
	return _params.CharHeight;
}

void Font::FillPosUv(std::vector<GLfloat> vec, unsigned int index, char c)
{
	if (0 == _params.NumWidth)
		return;

	if (0 == _params.NumHeight)
		return;

	float scal = 0.001f;
	float z = -1.0f;
	int startIndex = index * 5 * 6;
	int row = c / _params.NumWidth;
	int col = c % _params.NumWidth;
	auto du = 1.0f / _params.NumWidth;
	auto dv = 1.0f / _params.NumHeight;

	vec[startIndex + 0] = scal * index * (float)_params.GridSize; // x1
	vec[startIndex + 1] = scal * 0; // y1
	vec[startIndex + 2] = scal * z; // z
	vec[startIndex + 3] = scal * col * du; // u1
	vec[startIndex + 4] = scal * row * dv; // v1

	vec[startIndex + 5] = scal * index * (float)_params.GridSize; // x1
	vec[startIndex + 6] = scal * (float)_params.GridSize; // y2
	vec[startIndex + 7] = scal * z; // z
	vec[startIndex + 8] = scal * col * du; // u1
	vec[startIndex + 9] = scal * (row + 1) * dv; // v2

	vec[startIndex + 10] = scal * (index + 1) * (float)_params.GridSize; // x2
	vec[startIndex + 11] = scal * 0; // y1
	vec[startIndex + 12] = scal * z; // z
	vec[startIndex + 13] = scal * (col + 1) * du; // u2
	vec[startIndex + 14] = scal * row * dv; // v1

	vec[startIndex + 15] = scal * (index + 1) * (float)_params.GridSize; // x2
	vec[startIndex + 16] = scal * 0; // y1
	vec[startIndex + 17] = scal * z; // z
	vec[startIndex + 18] = scal * (col + 1) * du; // u2
	vec[startIndex + 19] = scal * row * dv; // v1

	vec[startIndex + 20] = scal * index * (float)_params.GridSize; // x1
	vec[startIndex + 21] = scal * (float)_params.GridSize; // y2
	vec[startIndex + 22] = scal * z; // z
	vec[startIndex + 23] = scal * col * du; // u1
	vec[startIndex + 24] = scal * (row + 1) * dv; // v2

	vec[startIndex + 25] = scal * index * (float)_params.GridSize; // x2
	vec[startIndex + 26] = scal * (float)_params.GridSize; // y2
	vec[startIndex + 27] = scal * z; // z
	vec[startIndex + 28] = scal * (col + 1) * du; // u2
	vec[startIndex + 29] = scal * (row + 1) * dv; // v2
}

int Font::GetCharNum(char c)
{
	if (c == 32)
		return (int)_params.SpaceChar;
	else if (c == 176)
		return (int)_params.DegreeChar;

	int charnum = ((int)c) - (int)Font::StartChar;

	if ((charnum >= 0) && (charnum < (int)(_params.NumWidth * _params.NumHeight)) && (charnum < (int)MaxChars))
		return charnum;

	return (int)_params.SpaceChar;
}

std::string Font::GetFontFilename(FontOptions::FontSize size)
{
	auto fontName = GetFontName(size);

	std::stringstream datafilenamess;
	datafilenamess << "./Resources/Fonts/" << fontName << ".txt";

	return datafilenamess.str();
}
