#include "Timer.h"

using namespace engine;

Timer::Timer()
{
}

Timer::~Timer()
{
}

Time Timer::GetTime()
{
	return std::chrono::steady_clock::now();
}

double Timer::GetElapsedSeconds(Time t1, Time t2)
{
	return std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count();
}

void Timer::Tick(unsigned int sampsIncrement, unsigned int loopCountIncrement)
{
	_loopCount += loopCountIncrement;
}

std::tuple<unsigned long, int> engine::Timer::QuantiseLength(unsigned long length)
{
	return std::make_tuple(length, 0);
}
