#pragma once

#include <memory>
#include "Audible.h"
#include "Sharable.h"

namespace base
{
	class MultiAudible :
		public virtual Sharable
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
		virtual MultiAudioDirection MultiAudibleDirection() const { return MULTIAUDIO_NONE; }
	};
}
