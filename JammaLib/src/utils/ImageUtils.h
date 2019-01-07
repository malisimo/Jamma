#pragma once

#include <string>
#include <vector>
#include <optional>

class ImageUtils
{
public:
	static std::optional<std::tuple<std::vector<unsigned char>, unsigned int, unsigned int>> LoadTga(const std::string& fileName);
};

