#include "AudioFader.h"

AudioFader::AudioFader() :
	_target(0.0f)
{
}

AudioFader::~AudioFader()
{
}

void AudioFader::SetTarget(float target)
{
	_target = target;
}

std::function<AudioFader::LinearState(
	const std::vector<float>& source,
	std::vector<float>& dest,
	unsigned int numSamps,
	float seekVal,
	AudioFader::LinearParams params,
	AudioFader::LinearState state)> AudioFader::Linear()
{
	return [](const std::vector<float>& source,
		std::vector<float>& dest,
		unsigned int numSamps,
		float seekVal,
		LinearParams params,
		LinearState state) 
	{
		auto nSrc = source.size();
		auto nDest = dest.size();

		if ((0 == nSrc) || (0 == nDest))
			return state;

		auto isComplete = false;

		if (!params.IsSourceLooped)
		{
			if (state.SourceIndex >= nSrc)
				isComplete = true;
		}

		if (!params.IsDestLooped)
		{
			if (state.DestIndex >= nDest)
				isComplete = true;
		}

		auto iSrc = params.IsSourceLooped ? state.SourceIndex % nSrc : state.SourceIndex;
		auto iDest = params.IsDestLooped ? state.DestIndex % nDest : state.DestIndex;
		auto currentVal = state.LastVal;

		if (state.EndVal != seekVal)
		{
			state.EndVal = seekVal;
			state.DVal = (state.EndVal - state.LastVal) * params.Rate;
		}

		for (auto samp = 0u; samp < numSamps; samp++)
		{
			if (state.DVal != 0.0f)
			{
				if (state.DVal > 0.0f)
				{
					if (currentVal >= seekVal)
					{
						currentVal = seekVal;
						state.DVal = 0.0f;
					}
				}
				else if (currentVal <= seekVal)
				{
					currentVal = seekVal;
					state.DVal = 0.0f;
				}

				currentVal += state.DVal;
			}

			if (!isComplete)
			{
				dest[iDest++] = currentVal * source[iSrc++];
				
				if (iSrc >= nSrc)
				{
					if (params.IsSourceLooped)
						while (iSrc >= nSrc)
							iSrc -= nSrc;
					else
						isComplete = true;
				}

				if (iDest >= nDest)
				{
					if (params.IsDestLooped)
						while (iDest >= nDest)
							iDest -= nDest;
					else
						isComplete = true;
				}
			}
		}

		state.SourceIndex += numSamps;

		if (params.IsSourceLooped)
			state.SourceIndex %= nSrc;

		state.DestIndex += numSamps;

		if (params.IsDestLooped)
			state.DestIndex %= nDest;

		state.LastVal = currentVal;

		return state;
	};
}

std::function<AudioFader::ExponentialState(
	const std::vector<float>& source,
	std::vector<float>& dest,
	unsigned int numSamps,
	unsigned int destIndex,
	float seekVal,
	AudioFader::ExponentialParams params,
	AudioFader::ExponentialState state)> AudioFader::Exponential()
{
	return[](const std::vector<float>& source,
		std::vector<float>& dest,
		unsigned int numSamps,
		unsigned int destIndex,
		float seekVal,
		ExponentialParams params,
		ExponentialState state)
	{
		auto nSrc = source.size();
		auto nDest = dest.size();

		if ((0 == nSrc) || (0 == nDest))
			return state;

		auto isComplete = false;
		
		if (!params.IsSourceLooped)
		{
			if (state.SourceIndex >= nSrc)
				isComplete = true;
		}

		if (!params.IsDestLooped)
		{
			if (destIndex >= nDest)
				isComplete = true;
		}

		auto iSrc = params.IsSourceLooped ? state.SourceIndex % nSrc : state.SourceIndex;
		auto iDest = params.IsDestLooped ? destIndex % nDest : destIndex;
		auto currentVal = state.LastVal;

		for (auto samp = 0u; samp < numSamps; samp++)
		{
			currentVal = (params.Damping * currentVal) + ((1.0f - params.Damping) * seekVal);
			
			if (!isComplete)
			{
				dest[iDest++] = currentVal * source[iSrc++];

				if (iSrc >= nSrc)
				{
					if (params.IsSourceLooped)
						while (iSrc >= nSrc)
							iSrc -= nSrc;
					else
						isComplete = true;
				}

				if (iDest >= nDest)
				{
					if (params.IsDestLooped)
						while (iDest >= nDest)
							iDest -= nDest;
					else
						isComplete = true;
				}
			}
		}

		state.SourceIndex += numSamps;

		if (params.IsSourceLooped)
			state.SourceIndex %= nSrc;

		state.DestIndex += numSamps;

		if (params.IsDestLooped)
			state.DestIndex %= nDest;

		state.LastVal = currentVal;

		return state;
	};
}

std::vector<float> AudioFader::OnAudio(unsigned int numSamps)
{
	auto out = std::vector<float>(numSamps);

	for (unsigned int index = 0; index < numSamps; index++)
	{
		out[index] = _target;
	}

	return out;
}