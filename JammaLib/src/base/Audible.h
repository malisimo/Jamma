#pragma once

namespace base
{
	class Audible :
		public std::enable_shared_from_this<Audible>
	{
	public:
		enum AudioDirection
		{
			AUDIO_NONE,
			AUDIO_SOURCE,
			AUDIO_SINK,
			AUDIO_BOTH
		};

	public:
		Audible() {};
		~Audible() {};

	public:
		virtual AudioDirection AudibleDirection() const
		{
			return AUDIO_NONE;
		}
	};
}
