#include "Timer.h"

using namespace engine;

Timer::Timer()
{
}

Timer::~Timer()
{
}

void Timer::Tick(unsigned int sampsIncrement, unsigned int loopCountIncrement)
{
	_loopCount += loopCountIncrement;
}