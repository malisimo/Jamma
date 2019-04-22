#include "InterpolatedValue.h"

using namespace audio;

InterpolatedValue::InterpolatedValue(InterpolatedValueParams faderParams) :
	_target(0.0f)
{
}

float InterpolatedValue::Next()
{
	return _target;
}

void InterpolatedValue::SetTarget(float target)
{
	_target = target;
}

InterpolatedValueLinear::InterpolatedValueLinear(InterpolatedValueLinear::LinearParams linearParams) :
	InterpolatedValue(linearParams),
	_params(linearParams)
{
}

float InterpolatedValueLinear::Next()
{
	auto currentVal = _lastVal;

	if (_dVal > 0.0f)
	{
		if (currentVal >= _target)
		{
			currentVal = _target;
			_dVal = 0.0f;
		}
	}
	else if (_dVal < 0.0f)
	{
		if (currentVal <= _target)
		{
			currentVal = _target;
			_dVal = 0.0f;
		}
	}

	_lastVal = currentVal + _dVal;

	return _lastVal;
}

void InterpolatedValueLinear::SetTarget(float target)
{
	if (_endVal != target)
	{
		_endVal = target;
		_dVal = (_endVal - _lastVal) * _params.Rate;
	}
}

InterpolatedValueExp::InterpolatedValueExp(InterpolatedValueExp::ExponentialParams expParams) :
	InterpolatedValue(expParams),
	_params(expParams)
{
}

float InterpolatedValueExp::Next()
{
	auto currentVal = _lastVal;
	_lastVal = (_params.Damping * currentVal) + ((1.0f - _params.Damping) * _target);

	return _lastVal;
}
