#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <memory>

namespace io
{
	class FileReadWriter
	{
	public:
		std::optional<std::tuple<std::vector<float>, unsigned int, unsigned int>>
			Read(const std::string& fileName, unsigned int maxSamps) const
		{
			return std::move(_Read(fileName, maxSamps));
		}

		bool Write(std::string fileName,
			std::vector<float> buffer,
			unsigned int numSamps,
			unsigned int sampleRate) const
		{
			return _Write(fileName,
				buffer,
				numSamps,
				sampleRate);
		}

	protected:
		virtual std::optional<std::tuple<std::vector<float>, unsigned int, unsigned int>>
			_Read(const std::string& fileName, unsigned int maxSamps) const = 0;

		virtual bool _Write(std::string fileName,
			std::vector<float> buffer,
			unsigned int numSamps,
			unsigned int sampleRate) const = 0;
	};
}
