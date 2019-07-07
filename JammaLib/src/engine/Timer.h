#pragma once

#include <chrono>
#include <tuple>

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
		bool IsMasterLengthSet() const { return 0 != _masterLength; }
		void SetMasterLength(unsigned long length) { _masterLength = length; }
		std::tuple<unsigned long, unsigned int> QuantiseLength(unsigned long length);

	private:
		unsigned long _loopCount;
		unsigned int sampOffset;
		unsigned long _masterLength;
	};
}

