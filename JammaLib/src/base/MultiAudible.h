#pragma once

#include <memory>
#include "Audible.h"

namespace base
{
	class MultiAudible :
		public std::enable_shared_from_this<MultiAudible>
	{
	public:
		enum MultiAudioDirection
		{
			MULTIAUDIO_NONE,
			MULTIAUDIO_SOURCE,
			MULTIAUDIO_SINK,
			MULTIAUDIO_BOTH
		};
	public:
		MultiAudible() {};
		~MultiAudible() {};

	public:
		virtual MultiAudioDirection MultiAudibleDirection() const
		{
			return MULTIAUDIO_NONE;
		}
	};
}
