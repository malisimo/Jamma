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

class ResourceLib
{
public:
	ResourceLib();

public:
	int NumResources() const;
	void ClearResources();
	bool LoadResource(Resources::Type type, std::string name, std::vector<std::string> args);
	std::optional<std::weak_ptr<Resource>> GetResource(const std::string& name);
	
	static std::optional<std::tuple<std::vector<unsigned char>, unsigned int, unsigned int>> LoadTga(const std::string& fileName);

private:
	std::map<std::string, std::shared_ptr<Resource>> _resources;
};

