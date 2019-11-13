#pragma once

#include <vector>
#include <memory>

namespace audio
{
	class FallingValue
	{
	public:
		class FallingValueParams
		{
		public:
			double FallRate;
		};

	public:
		FallingValue(FallingValueParams fallingParams);

	public:
		virtual double Next();
		virtual double Current() const;
		virtual void SetTarget(double target);

	protected:
		double _target;
		double _lastValue;
		FallingValueParams _fallingParams;
	};
}
