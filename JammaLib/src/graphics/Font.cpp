#include "Font.h"

// Font class to load and set up individual fonts, and 
// FontRenderer to hold a library of fonts with utility methods

//
// Font
//

Font::Font()
{
}

Font::Font(FontOptions::FontParams params,
	std::vector<float> charWidths,
	std::weak_ptr<TextureResource> texture) :
	_params(params),
	_charWidths(charWidths),
	_texture(texture)
{
}

std::optional<std::unique_ptr<Font>> Font::Load(FontOptions::FontSize size, ResourceLib& resourceLib)
{
	// Load up the texture AND the text file
	auto fontName = GetFontName(size);
	auto resourceOpt = resourceLib.GetResource(fontName);

	if (!resourceOpt.has_value())
		return std::nullopt;

	auto resource = resourceOpt.value().lock();
	if (!resource)
		return std::nullopt;

	if (!Resources::TEXTURE == resource->GetType())
		return std::nullopt;

	auto texture = std::dynamic_pointer_cast<TextureResource>(resource);

	auto datafilename = GetDataFilename(size);

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
	unsigned int gridSize;
	unsigned int spaceChar;
	unsigned int degreeChar;
	float charHeight;
	std::vector<float> charWidths;

	std::string line;

	// While we are able to get lines from the file
	while (std::getline(inputFile, line))
	{
		linenum++;

		if (linenum > 4)
			break;

		// Parse the line from the file and
		// set appropriate parameters
		std::stringstream ss(line);

		if (linenum == 1)
		{
			while (ss >> param2)
			{
				charWidths.push_back(param2 + 1.0f);
			}
		}
		else if (linenum == 2)
		{
			if (ss >> param2)
				charHeight = param2;
		}
		else if (linenum == 3)
		{
			if (ss >> param1)
				gridSize = param1;
		}
		else if (linenum == 4)
		{
			if (ss >> param1)
				spaceChar = param1;
		}
		else if (linenum == 5)
		{
			if (ss >> param1)
				degreeChar = param1;
		}
	}

	auto numWidth = (unsigned int)(((double)texture->Width()) / ((double)gridSize));
	auto numHeight = (unsigned int)(((double)texture->Height()) / ((double)gridSize));

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

		FontOptions::FontParams fontParams = { gridSize, charHeight, (unsigned int)numChars, spaceChar, degreeChar, size };
		return std::make_unique<Font>(fontParams, charWidths, texture);
	}

	return std::nullopt;
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
	int startIndex = index * 5;
	int row = c / _numWidth;
	int col = c % _numWidth;

	auto du = 1.0f;
	auto dv = 1.0f;

	if (_numWidth > 0)
		du /= _numWidth;
	if (_numHeight > 0)
		dv /= _numHeight;

	vec[startIndex + 0] = index * (float)_params.GridSize; // x1
	vec[startIndex + 1] = 0; // y1
	vec[startIndex + 2] = 0; // z
	vec[startIndex + 3] = col * du; // u1
	vec[startIndex + 4] = row * dv; // v1

	vec[startIndex + 5] = index * (float)_params.GridSize; // x1
	vec[startIndex + 6] = (float)_params.GridSize; // y2
	vec[startIndex + 7] = 0; // z
	vec[startIndex + 8] = col * du; // u1
	vec[startIndex + 9] = (row + 1) * dv; // v2

	vec[startIndex + 10] = (index + 1) * (float)_params.GridSize; // x2
	vec[startIndex + 11] = 0; // y1
	vec[startIndex + 12] = 0; // z
	vec[startIndex + 13] = (col + 1) * du; // u2
	vec[startIndex + 14] = row * dv; // v1

	vec[startIndex + 15] = (index + 1) * (float)_params.GridSize; // x2
	vec[startIndex + 16] = 0; // y1
	vec[startIndex + 17] = 0; // z
	vec[startIndex + 18] = (col + 1) * du; // u2
	vec[startIndex + 19] = row * dv; // v1

	vec[startIndex + 0] = index * (float)_params.GridSize; // x1
	vec[startIndex + 1] = (float)_params.GridSize; // y2
	vec[startIndex + 2] = 0; // z
	vec[startIndex + 3] = col * du; // u1
	vec[startIndex + 4] = (row + 1) * dv; // v2

	vec[startIndex + 20] = index * (float)_params.GridSize; // x2
	vec[startIndex + 21] = (float)_params.GridSize; // y2
	vec[startIndex + 22] = 0; // z
	vec[startIndex + 23] = (col + 1) * du; // u2
	vec[startIndex + 24] = (row + 1) * dv; // v2
}

int Font::GetCharNum(char c)
{
	if (c == 32)
		return (int)_params.SpaceChar;
	else if (c == 176)
		return (int)_params.DegreeChar;

	int charnum = ((int)c) - (int)Font::StartChar;

	if ((charnum >= 0) && (charnum < (int)_params.NumChars) && (charnum < (int)MaxChars))
		return charnum;

	return (int)_params.SpaceChar;
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

std::string Font::GetTextureFilename(FontOptions::FontSize size)
{
	auto fontName = GetFontName(size);
	
	std::stringstream texturefilenamess;
	texturefilenamess << "./Textures/" << fontName << ".png";

	return texturefilenamess.str();
}

std::string Font::GetDataFilename(FontOptions::FontSize size)
{
	auto fontName = GetFontName(size);

	std::stringstream datafilenamess;
	datafilenamess << "./Resources/" << fontName << ".txt";

	return datafilenamess.str();
}


//
// FontRenderer
//

FontRenderer::FontRenderer() :
	_loadedFonts(false)
{
}

FontRenderer::~FontRenderer()
{
	_fonts.clear();
}

bool FontRenderer::Init(ResourceLib& resourceLib)
{
	int numbytes = Font::MaxVerts * 3 * sizeof(float);
	return LoadFonts(resourceLib);
}

float FontRenderer::MeasureString(const std::string& str, FontOptions::FontSize size)
{
	auto fontOpt = GetFont(size);

	if (!fontOpt.has_value())
		return 0;

	return fontOpt.value()->MeasureString(str);
}

float FontRenderer::FontHeight(FontOptions::FontSize size)
{
	auto fontOpt = GetFont(size);

	if (!fontOpt.has_value())
		return 0;

	return fontOpt.value()->GetHeight();
}

std::optional<Font*> FontRenderer::GetFont(FontOptions::FontSize size)
{
	if (_fonts.count(size) > 0)
	{
		auto font = _fonts.at(size).get();
		if (nullptr == font)
			return std::nullopt;

		return font;
	}

	return std::nullopt;
}

bool FontRenderer::LoadFonts(ResourceLib& resourceLib)
{
	if (_fonts.size() >= (sizeof(FontOptions::FontSizes) / sizeof(int)))
		return true;

	_fonts.clear();

	bool res = false;
	for (auto size : FontOptions::FontSizes)
	{
		auto font = Font::Load(size, resourceLib);
		FontOptions::FontParams fontParams{ 1, 2.f, 3, 4, 5, FontOptions::FONT_TINY };

		if (font.has_value())
			_fonts[size].swap(font.value());
		else
			res = false;
	}

	return res;
}
