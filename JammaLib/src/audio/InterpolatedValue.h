#pragma once

#include <vector>
#include <memory>
#include <functional>
#include "AudioSource.h"
#include "AudioBuffer.h"

namespace audio
{
	class InterpolatedValue
	{
	public:
		class InterpolatedValueParams {};

	public:
		InterpolatedValue(InterpolatedValueParams faderParams);

	public:
		virtual double Next();
		virtual void SetTarget(double target);

	protected:
		double _target;
	};

	class InterpolatedValueLinear : public InterpolatedValue
	{
	public:
		class LinearParams : public InterpolatedValueParams
		{
		public:
			double Rate;
		};

	public:
		InterpolatedValueLinear();
		InterpolatedValueLinear(LinearParams linearParams);

	public:
		virtual double Next() override;
		virtual void SetTarget(double target) override;

	protected:
		double _endVal;
		double _dVal;
		double _lastVal;
		LinearParams _params;
	};

	class InterpolatedValueExp : public InterpolatedValue
	{
	public:
		class ExponentialParams : public InterpolatedValueParams
		{
		public:
			double Damping;
		};

	public:
		InterpolatedValueExp();
		InterpolatedValueExp(ExponentialParams expParams);

	public:
		virtual double Next() override;

	protected:
		double _lastVal;
		ExponentialParams _params;
	};
}
