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
#include "../utils/FunctionUtils.h"

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
		float CharHeight;
		unsigned int NumChars;
		unsigned int SpaceChar;
		unsigned int DegreeChar;
		FontSize Size;
	};
}

class Font
{
public:
	Font();
	Font(FontOptions::FontParams params,
		std::vector<float> charWidths,
		std::weak_ptr<TextureResource> texture);

	// Copy
	Font(const Font &) = delete;
	Font& operator=(const Font &) = delete;

	// Move
	Font(Font &&other) :
		_params(other._params),
		_numWidth(other._numWidth),
		_numHeight(other._numHeight),
		_charWidths(std::move(other._charWidths)),
		_texture(other._texture)
	{
		other._params = {};
		other._numWidth = 0;
		other._numHeight = 0;
		other._charWidths = {};
		other._texture = std::weak_ptr<TextureResource>();
	}

	Font& operator=(Font &&other)
	{
		if (this != &other)
		{
			std::swap(_params, other._params);
			std::swap(_numWidth, other._numWidth);
			std::swap(_numHeight, other._numHeight);
			std::swap(_charWidths, other._charWidths);
			_texture.swap(other._texture);
		}

		return *this;
	}

	GLuint InitVertexArray(const std::string& str, GLenum usage);
	float MeasureString(const std::string& str);
	float GetHeight();

	static std::optional<std::unique_ptr<Font>> Load(FontOptions::FontSize size, ResourceLib& resourceLib);

private:
	void FillPosUv(std::vector<GLfloat> vec, unsigned int index, char c);
	int GetCharNum(char c);

	static std::string GetFontName(FontOptions::FontSize size);
	static std::string GetTextureFilename(FontOptions::FontSize size);
	static std::string GetDataFilename(FontOptions::FontSize size);

public:
	static const unsigned int MaxChars = 256;
	static const unsigned int MaxVerts = 1536;
	static const unsigned int StartChar = 33;

private:
	FontOptions::FontParams _params;
	unsigned int _numWidth;
	unsigned int _numHeight;
	std::vector<float> _charWidths;
	std::weak_ptr<TextureResource> _texture;
};


class FontRenderer
{
public:
	FontRenderer();
	~FontRenderer();

	bool Init(ResourceLib& resourceLib);
	float MeasureString(const std::string& str, FontOptions::FontSize size);
	float FontHeight(FontOptions::FontSize size);

private:
	std::optional<Font*> GetFont(FontOptions::FontSize size);
	bool LoadFonts(ResourceLib& resourceLib);

private:
	bool _loadedFonts;
	std::map<FontOptions::FontSize, std::unique_ptr<Font>> _fonts;
};
