#pragma once

#include <vector>
#include <memory>

namespace base
{
	class AudioSink
	{
	public:
		virtual void Push(const float& samp) {};
		virtual void PushMix(const float& samp) {};
	};
}
