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
#include "../include/Constants.h"
#include "../utils/MathUtils.h"

namespace io
{
	struct UserConfig
	{
		static std::optional<UserConfig> FromJson(Json::JsonPart json);

		struct AudioSettings
		{
			std::string Name; // The name of the device (used for matching rig preferences)
			unsigned int BufSize; // The buffer size used by the device
			unsigned int Latency; // he overall rountrip IO latency, in samples
			unsigned int NumChannelsIn; // The number of input channels used in current scene
			unsigned int NumChannelsOut; // The number of output channels used in current scene

			static std::optional<AudioSettings> FromJson(Json::JsonPart json);
		};

		struct LoopSettings
		{
			unsigned int FadeSamps; // The number of samples to fade in/out the start/end of a loop

			static std::optional<LoopSettings> FromJson(Json::JsonPart json);
		};

		struct TriggerSettings
		{
			unsigned int PreDelay; // How many samples to push trigger point to left (positive is earlier trig)
			unsigned int DebounceSamps; // How many samples over which to prevent trigger bounce

			static std::optional<TriggerSettings> FromJson(Json::JsonPart json);
		};

		// How much to (further) delay input signal from ADC, in samples
		unsigned int AdcBufferDelay() const {
			return Trigger.PreDelay + constants::MaxLoopFadeSamps - Audio.Latency;
		}

		// How long to continue recording after trigger to end loop recording, in samples
		unsigned int EndRecordingSamps(int error) const {
			if (error > 0)
			{
				if (error > (int)constants::MaxLoopFadeSamps)
					return 0;
			}

			return constants::MaxLoopFadeSamps - error;
		}

		// The index at which to start playing a loop after trigger to end recording,
		// in samples (includes intro, so zero is first index of intro, not the loop)
		unsigned long LoopPlayPos(int error, unsigned long loopLength) const {
			auto loopSamp = utils::ModNeg(
				((long)Trigger.PreDelay) - (long)(constants::MaxLoopFadeSamps - Audio.Latency),
				loopLength);

			return loopSamp;
		}

		AudioSettings Audio;
		LoopSettings Loop;
		TriggerSettings Trigger;
	};
}
