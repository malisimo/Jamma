#pragma once

#include <vector>
#include <map>
#include <optional>
#include <memory>
#include <gl/glew.h>
#include <gl/gl.h>
#include "Resource.h"
#include "TextureResource.h"
#include "ShaderResource.h"
#include "../graphics/Font.h"

class ResourceLib
{
public:
	ResourceLib();

public:
	int NumResources() const;
	void ClearResources();
	bool LoadResource(Resources::Type type, std::string name, std::vector<std::string> args);
	std::optional<std::weak_ptr<Resource>> GetResource(const std::string& name);
	bool LoadFonts();
	std::optional<std::weak_ptr<Font>> GetFont(FontOptions::FontSize size);

private:

	std::map<std::string, std::shared_ptr<Resource>> _resources;
	std::map<FontOptions::FontSize, std::shared_ptr<Font>> _fonts;
};

