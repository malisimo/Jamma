#pragma once

#include <vector>
#include <memory>

namespace base
{
	class AudioSink
	{
	public:
		AudioSink() : _index(0) {}

	public:
		inline virtual int Write(float samp, int indexOffset) { return indexOffset; };
		inline virtual int WriteMix(float samp, int indexOffset) { return indexOffset; };
		inline virtual void Offset(int indexOffset) { _index+= indexOffset; };

	protected:
		unsigned int _index;
	};
}
