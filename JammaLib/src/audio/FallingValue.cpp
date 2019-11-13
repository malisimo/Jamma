#include "FallingValue.h"

using namespace audio;

FallingValue::FallingValue(FallingValueParams fallingParams) :
	_target(0.0),
	_lastValue(0.0),
	_fallingParams(fallingParams)
{
}

double FallingValue::Next()
{
	auto curValue = _lastValue;
	auto next = _lastValue - _fallingParams.FallRate;

	_lastValue = next < _target ? _target : next;
	return curValue;
}

double FallingValue::Current() const
{
	return _lastValue;
}

void FallingValue::SetTarget(double target)
{
	_target = std::abs(target);

	if (_lastValue < _target)
		_lastValue = _target;
}
