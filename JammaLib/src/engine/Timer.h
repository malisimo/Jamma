#pragma once

#include <chrono>

typedef std::chrono::time_point<std::chrono::steady_clock> Time;

namespace engine
{
	class Timer
	{
	public:
		Timer();
		~Timer();

	public:
		static Time GetTime();
		static double GetElapsedSeconds(Time t1, Time t2);

		void Tick(unsigned int sampsIncrement, unsigned int loopCountIncrement);

	private:
		unsigned long _loopCount;
		unsigned int sampOffset;
	};
}

