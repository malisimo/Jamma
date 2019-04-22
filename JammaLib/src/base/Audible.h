#pragma once

#include <vector>
#include <memory>

namespace audio
{
	class AudioBuffer;
}

namespace base
{
	class AudibleParams
	{
	};

	class Audible
	{
	public:
		Audible(AudibleParams params);
		~Audible();

	public:
		virtual void Play(std::shared_ptr<audio::AudioBuffer> buf, unsigned int numSamps);

	protected:
		AudibleParams _audibleParams;
	};
}
