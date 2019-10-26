///////////////////////////////////////////////////////////
//
// Author 2019 Matt Jones
// Subject to the MIT license, see LICENSE file.
//
///////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <stack>
#include <map>
#include <optional>
#include <variant>
#include <iostream>
#include <sstream>
#include "Json.h"

namespace io
{
	struct UserConfig
	{
		static std::optional<UserConfig> FromJson(Json::JsonPart json);

		struct AudioSettings
		{
			std::string Name;
			unsigned int BufSize;
			unsigned int Latency;
			unsigned int NumChannelsIn;
			unsigned int NumChannelsOut;

			static std::optional<AudioSettings> FromJson(Json::JsonPart json);
		};

		struct LoopSettings
		{
			unsigned int FadeSamps;

			static std::optional<LoopSettings> FromJson(Json::JsonPart json);
		};

		struct TriggerSettings
		{
			unsigned int PreDelay;
			unsigned int DebounceSamps;

			static std::optional<TriggerSettings> FromJson(Json::JsonPart json);
		};

		std::string Name;
		AudioSettings Audio;
		LoopSettings Loop;
		TriggerSettings Trigger;
	};
}
