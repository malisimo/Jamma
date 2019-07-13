#pragma once

#include <vector>
#include <memory>
#include "Audible.h"

namespace base
{
	class AudioSink :
		public virtual Audible
	{
	public:
		AudioSink() : _index(0) {}

	public:
		virtual AudioDirection AudibleDirection() const override { return AUDIO_SINK; }
		inline virtual int OnWrite(float samp, int indexOffset) { return indexOffset; };
		inline virtual void Offset(int indexOffset) { _index+= indexOffset; };

		std::shared_ptr<AudioSink> shared_from_this()
		{
			return std::dynamic_pointer_cast<AudioSink>(
				Audible::shared_from_this());
		}

	protected:
		unsigned int _index;
	};
}
