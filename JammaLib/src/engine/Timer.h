#pragma once

namespace engine
{
	class Timer
	{
	public:
		Timer();
		~Timer();

	public:
		void Tick(unsigned int sampsIncrement, unsigned int loopCountIncrement);

	private:
		unsigned long _loopCount;
		unsigned int sampOffset;
	};
}

