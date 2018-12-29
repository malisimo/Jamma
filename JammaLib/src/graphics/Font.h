#pragma once

///////////////////////////////////////////////////////////////
//
// Font header file
// ================
//
// Contains definition of font character widths
//
///////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <gl/glew.h>
#include <gl/gl.h>
#include "Drawable.h"
#include "../resources/TextureResource.h"

namespace FontOptions
{
	enum FontSize
	{
		FONT_TINY = 0,
		FONT_SMALL = 1,
		FONT_MEDIUM = 2,
		FONT_LARGE = 3
	};

	static const FontOptions::FontSize FontSizes[] = { FontOptions::FONT_LARGE, FontOptions::FONT_MEDIUM, FontOptions::FONT_SMALL, FontOptions::FONT_TINY };

	enum TextAlign
	{
		TEXTALIGN_LEFT = 0,
		TEXTALIGN_RIGHT = 1,
		TEXTALIGN_CENTRE = 2
	};

	struct Colour
	{
	public:
		float R;
		float G;
		float B;
	};

	static const Colour White{ 1.0f, 1.0f, 1.0f };

	struct FontParams
	{
		unsigned int GridSize;
		std::vector<float> CharWidths;
		float CharHeight;
		unsigned int NumChars;
		unsigned int SpaceChar;
		unsigned int DegreeChar;
		FontSize Size;
		std::weak_ptr<TextureResource> Texture;
	};
}

class Font
{
public:
	Font();
	Font(FontOptions::FontParams params);

	void InitQuad(char c, float x, float y, float* vert_ptr);
	int GetCharNum(char c);

	static std::optional<std::unique_ptr<Font>> Load(FontOptions::FontSize size, ResourceLib& resourceLib);

private:
	static std::string GetFontName(FontOptions::FontSize size);
	static std::string GetTextureFilename(FontOptions::FontSize size);
	static std::string GetDataFilename(FontOptions::FontSize size);

public:

	static const unsigned int MaxChars = 256;
	static const unsigned int MaxVerts = 1536;
	static const unsigned int StartChar = 33;

public:
	unsigned int GridSize;
	std::vector<float> CharWidths;
	float CharHeight;
	unsigned int NumChars;
	unsigned int SpaceChar;
	unsigned int DegreeChar;
	FontOptions::FontSize Size;

private:
	std::weak_ptr<TextureResource> _texture;
	unsigned int _numWidth;
	unsigned int _numHeight;
};


class FontRenderer
{
public:
	FontRenderer();
	~FontRenderer();

	bool Init(ResourceLib& resourceLib);
	void Reset();

	void DrawString(std::string str, float x, float y, FontOptions::FontSize size);
	void DrawString(std::string str, float x, float y, float rotation, FontOptions::FontSize size);
	void DrawString(std::string str, float x, float y, FontOptions::FontSize size, FontOptions::Colour colour);
	void DrawString(std::string str, float x, float y, float rotation, FontOptions::FontSize size, FontOptions::Colour colour);

	float MeasureString(std::string str, FontOptions::FontSize size);
	float FontHeight(FontOptions::FontSize size);

private:
	std::optional<Font*> GetFont(FontOptions::FontSize size);
	bool LoadFonts(ResourceLib& resourceLib);

private:
	bool _loadedFonts;
	std::map<FontOptions::FontSize, std::unique_ptr<Font>> _fonts;
	GLuint _vertexBuffer;
};
