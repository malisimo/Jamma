#pragma once

#include <vector>
#include <memory>
#include <functional>
#include "Audible.h"
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
		virtual float Next();
		virtual void SetTarget(float target);

	protected:
		float _target;
	};

	class InterpolatedValueLinear : public InterpolatedValue
	{
	public:
		class LinearParams : public InterpolatedValueParams
		{
		public:
			float Rate;
		};

	public:
		InterpolatedValueLinear(LinearParams linearParams);

	public:
		virtual float Next() override;
		virtual void SetTarget(float target) override;

	protected:
		float _endVal;
		float _dVal;
		float _lastVal;
		LinearParams _params;
	};

	class InterpolatedValueExp : public InterpolatedValue
	{
	public:
		class ExponentialParams : public InterpolatedValueParams
		{
		public:
			float Damping;
		};

	public:
		InterpolatedValueExp(ExponentialParams expParams);

	public:
		virtual float Next() override;

	protected:
		float _lastVal;
		ExponentialParams _params;
	};
}
