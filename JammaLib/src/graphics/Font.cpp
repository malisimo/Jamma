#include "Font.h"

// Font class to load and set up individual fonts, and 
// FontRenderer to provide API for drawing strings

//
// Font
//

Font::Font(FontOptions::FontParams params) :
	GridSize(params.GridSize),
	CharWidths(params.CharWidths),
	CharHeight(params.CharHeight),
	NumChars(params.NumChars),
	SpaceChar(params.SpaceChar),
	DegreeChar(params.DegreeChar),
	Size(params.Size),
	_texture(params.Texture)
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

		FontOptions::FontParams fontParams = { gridSize, charWidths, charHeight, (unsigned int)numChars, spaceChar, degreeChar, size, texture };
		return std::make_unique<Font>(fontParams);
	}

	return std::nullopt;
}

void Font::InitQuad(char c, float x, float y, float* vert_ptr)
{
	auto widthF = 1.0f;
	auto heightF = 1.0f;
	auto texture = _texture.lock();

	if (texture)
	{
		widthF = (float)texture->Width();
		heightF = (float)texture->Height();
	}

	float x2 = x + (float)GridSize;
	float y2 = y + (float)GridSize;
	int charnum = GetCharNum(c);

	if (charnum >= 0)
	{
		unsigned char uchar = (unsigned char)charnum;
		float tu, tv;
		float tu2, tv2;
		float numWidthF = (float)_numWidth;
		float numheightf = (float)_numHeight;
		float chardu = (((float)(_numWidth * GridSize)) / widthF) / numWidthF;
		float chardv = (((float)(_numHeight * GridSize)) / heightF) / numheightf;

		if (_numWidth == 0)
			numWidthF = 1.0f;

		if (_numHeight == 0)
			numheightf = 1.0f;

		int row = uchar / _numWidth;
		int col = uchar % _numWidth;

		float width_scale = 1.0f;//((float)CharWidths[charnum]) / ((float)GridSize);
		float height_scale = 1.0f;//((float)CharHeight) / ((float)GridSize);

		tu = chardu * ((float)col);
		tu2 = tu + (chardu * width_scale);

		tv = chardv * ((float)row);
		tv2 = tv + (chardv * height_scale);

		vert_ptr[0] = x;// x, y2, tu, tv2);
		vert_ptr[1] = y; // x, y, tu, tv);
		vert_ptr[2] = x2;// x2, y, tu2, tv);
		vert_ptr[3] = vert_ptr[0];
		vert_ptr[4] = vert_ptr[2];
		vert_ptr[5] = y2;// x2, y2, tu2, tv2);
	}
	else
	{
		vert_ptr[0] = x;// x, y2, 0.0f, 0.0f);
		vert_ptr[1] = y;// x, y, 0.0f, 0.0f);
		vert_ptr[2] = x2;// x2, y, 0.0f, 0.0f);
		vert_ptr[3] = vert_ptr[0];
		vert_ptr[4] = vert_ptr[2];
		vert_ptr[5] = y2;// x2, y2, 0.0f, 0.0f);
	}
}

int Font::GetCharNum(char c)
{
	if (c == 32)
		return (int)SpaceChar;
	else if (c == 176)
		return (int)DegreeChar;

	int charnum = ((int)c) - (int)Font::StartChar;

	if ((charnum >= 0) && (charnum < (int)NumChars) && (charnum < (int)MaxChars))
		return charnum;

	return (int)SpaceChar;
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
	_loadedFonts(false),
	_vertexBuffer(0)
{
}

FontRenderer::~FontRenderer()
{
	_fonts.clear();
}

bool FontRenderer::Init(ResourceLib& resourceLib)
{
	int numbytes = Font::MaxVerts * 3 * sizeof(float);
	auto res = LoadFonts(resourceLib);

	// device->CreateVertexBuffer(Font::MaxVerts * sizeof(D3DVERTEX), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &_vertexBuffer, NULL);

	/*if (SUCCEEDED(hr))
	{

		std::stringstream ss;
		ss << ExecutablePath << "\\Textures\\empty.png";

		hr = D3DXCreateTextureFromFile(device, ss.str().c_str(), &_fadeTexture);

		if (SUCCEEDED(hr))
			res = true;
		else
			_fadeTexture = NULL;
	}*/

	return res;
}

void FontRenderer::Reset()
{
	for (auto size : FontOptions::FontSizes)
	{
		//if (_fonts.count(size) > 0)
			//_fonts[size].Reset();
	}

	if (_vertexBuffer > 0)
	{
		glDeleteBuffers(1, &_vertexBuffer);
		_vertexBuffer = 0;
	}
}

void FontRenderer::DrawString(std::string str, float x, float y, FontOptions::FontSize size)
{
	DrawString(str, x, y, 0.0f, size, FontOptions::White);
}

void FontRenderer::DrawString(std::string str, float x, float y, float rotation, FontOptions::FontSize size)
{
	DrawString(str, x, y, rotation, size, FontOptions::White);
}

void FontRenderer::DrawString(std::string str, float x, float y, FontOptions::FontSize size, FontOptions::Colour colour)
{
	DrawString(str, x, y, 0.0f, size, colour);
}

void FontRenderer::DrawString(std::string str, float x, float y, float rotation, FontOptions::FontSize size, FontOptions::Colour colour)
{
	auto fontOpt = GetFont(size);

	if (!fontOpt.has_value())
		return;

	auto font = fontOpt.value();

	if (rotation == 0)
	{
		x = 0.5f + (float)(int)(x + 0.5f);
		y = 0.5f + (float)(int)(y + 0.5f);
	}

	int charnum;
	float* vert_ptr = nullptr;
	float currentx = 0.0f;
	unsigned int counter1;
	const char* cstr = str.c_str();
	unsigned int len = (unsigned int)strlen(cstr);

	int numbytestolock = len * 6 * 3 * sizeof(float);

	auto hr = true;// _vertexBuffer->Lock(0, numbytestolock, (void **)&vert_ptr, D3DLOCK_DISCARD);

	for (counter1 = 0; counter1<len; counter1++)
	{
		charnum = font->GetCharNum(cstr[counter1]);

		if (charnum >= 0)
		{
			font->InitQuad(cstr[counter1], currentx, 0, vert_ptr);

			currentx += (float)((int)(font->CharWidths[charnum] + 0.5f));

			vert_ptr += 6;
		}
	}

	//hr = _vertexBuffer->Unlock();

	//Shader* texshader = TextureResource::TexShader();

	//device->SetTexture(0, font->Texture());

	/*if (texshader)
	{
		float posVec[2];
		float posMat[4];
		float texRect[4];

		float xf = (float)x;
		float yf = (float)y;
		float wwf = (float)WindowWidth;
		float whf = (float)WindowHeight;

		posVec[0] = -1.0f * ((xf / (wwf * 0.5f)) - 1.0f);
		posVec[1] = -1.0f * ((yf / (whf * 0.5f)) - 1.0f);

		if (rotation == 0)
		{
			posMat[0] = 2.0f / wwf;
			posMat[1] = 0.0f;
			posMat[2] = 0.0f;
			posMat[3] = -2.0f / whf;
		}
		else
		{
			double wwd = (double)WindowWidth;
			double whd = (double)WindowHeight;
			double c = cos(((double)rotation) * DEG2RAD);
			double s = sin(((double)rotation) * DEG2RAD);

			posMat[0] = (float)(2.0 * c / wwd);
			posMat[1] = (float)(-2.0 * s / wwd);
			posMat[2] = (float)(2.0 * -s / whd);
			posMat[3] = (float)(-2.0 * c / whd);
		}

		// Set the texture coordinates
		texRect[0] = 0.0f;
		texRect[1] = 0.0f;
		texRect[2] = 1.0f;
		texRect[3] = 1.0f;

		texshader->Apply(posVec,
			posMat,
			texRect,
			colour);
	}*/

	//device->SetStreamSource(0, _vertexBuffer, 0, sizeof(D3DVERTEX));
	//device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, len * 2);
}


float FontRenderer::MeasureString(std::string str, FontOptions::FontSize size)
{
	auto fontOpt = GetFont(size);

	if (!fontOpt.has_value())
		return 0;

	auto font = fontOpt.value();

	const char* chars = str.c_str();
	unsigned int len = (unsigned int)strlen(chars);

	unsigned int counter1;
	float totallength = 0.0f;
	int charnum;

	for (counter1 = 0; counter1<len; counter1++)
	{
		charnum = font->GetCharNum(chars[counter1]);

		if (charnum >= 0)
			totallength += (float)((int)(font->CharWidths[charnum] + 0.5f));
	}

	return totallength;
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

float FontRenderer::FontHeight(FontOptions::FontSize size)
{
	if (_fonts.count(size) > 0)
		return _fonts.at(size)->CharHeight;

	return 12.0f;
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
		FontOptions::FontParams fontParams{ 1, {}, 3, 4, 5, 6, FontOptions::FONT_TINY, std::weak_ptr<TextureResource>() };

		if (font.has_value())
			_fonts.emplace(size,std::make_unique<Font>(fontParams));
		else
			res = false;
	}

	return res;
}
