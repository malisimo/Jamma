#pragma once

#include <vector>
#include <memory>
#include <functional>
#include "Audible.h"
#include "AudioBuffer.h"

class AudioFader : Audible
{
public:
	AudioFader();
	~AudioFader();

public:
	void Play(std::shared_ptr<AudioBuffer> buf, unsigned int samps) override;

	void SetTarget(float target);

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

protected:
	float _target;
	std::shared_ptr<Audible> _input;
};

