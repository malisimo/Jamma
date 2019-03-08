#pragma once

#include <vector>
#include <functional>

class AudioFader
{
public:
	AudioFader();
	~AudioFader();

public:

	// Base
	class AudioFaderState
	{
	public:
		unsigned int SourceIndex;
		unsigned int DestIndex;
		float LastVal;
	};

	class AudioFaderParams
	{
	public:
		bool IsSourceLooped;
		bool IsDestLooped;
	};
	
	// Linear
	class LinearState : public AudioFaderState
	{
	public:
		float EndVal;
		float DVal;
	};

	class LinearParams : public AudioFaderParams
	{
	public:
		float Rate;
	};

	static std::function<LinearState(
		const std::vector<float>& source,
		std::vector<float>& dest,
		unsigned int numSamps,
		float seekVal,
		LinearParams params,
		LinearState state)> Linear();

	// Exponential
	class ExponentialState : public AudioFaderState
	{
	public:
		float LastVal;
	};

	class ExponentialParams : public AudioFaderParams
	{
	public:
		float Damping;
	};

	static std::function<ExponentialState(
		const std::vector<float>& source,
		std::vector<float>& dest,
		unsigned int bufSize,
		unsigned int destIndex,
		float seekVal,
		ExponentialParams params,
		ExponentialState state)> Exponential();

	void SetTarget(float target);
	virtual std::vector<float> OnAudio(unsigned int numSamps);

protected:
	float _target;
};

