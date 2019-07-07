#include "ChannelMixer.h"

using namespace audio;
using namespace base;

ChannelMixer::ChannelMixer(ChannelMixerParams chanMixParams) :
	_inputBuffers(),
	_outputBuffers()
{
	SetParams(chanMixParams);
}

ChannelMixer::~ChannelMixer()
{
}

void ChannelMixer::SetParams(ChannelMixerParams chanMixParams)
{
	auto numInputs = (unsigned int)_inputBuffers.size();

	if (chanMixParams.NumInputChannels > numInputs)
	{
		for (auto i = 0u; i < chanMixParams.NumInputChannels - numInputs; i++)
		{
			_inputBuffers.push_back(std::make_unique<AudioBuffer>(chanMixParams.InputBufferSize));
		}
	}

	if (chanMixParams.NumInputChannels < numInputs)
		_inputBuffers.resize(numInputs);

	auto numOutputs = (unsigned int)_outputBuffers.size();

	if (chanMixParams.NumOutputChannels > numOutputs)
	{
		for (auto i = 0u; i < chanMixParams.NumOutputChannels - numOutputs; i++)
		{
			_outputBuffers.push_back(std::make_unique<AudioBuffer>(chanMixParams.OutputBufferSize));
		}
	}

	if (chanMixParams.NumOutputChannels < numOutputs)
		_outputBuffers.resize(numInputs);

	for (auto& buf : _inputBuffers)
		buf->SetSize(chanMixParams.OutputBufferSize);

	for (auto& buf : _inputBuffers)
		buf->SetSize(chanMixParams.OutputBufferSize);
}

void ChannelMixer::FromAdc(float* inBuf, unsigned int numChannels, unsigned int numSamps)
{
	if (numSamps < 1 || numChannels < 1)
		return;

	auto chan = 0u;
	for (auto& buf : _inputBuffers)
	{
		chan++;

		if (numChannels > chan)
		{
			auto currentOffset = 0;

			for (auto samp = 0u; samp < numSamps; samp++)
			{
				currentOffset = buf->OnWrite(inBuf[samp*numChannels + chan], currentOffset);
			}

			buf->Offset(numSamps);
		}
	}
}

void ChannelMixer::ToDac(float* outBuf, unsigned int numChannels, unsigned int numSamps)
{
	if (numSamps < 1 || numChannels < 1)
		return;

	auto chan = 0u;
	for (auto& buf : _outputBuffers)
	{
		chan++;

		if ((numChannels > chan) && (buf->BufSize() > 0))
		{
			auto bufIter = buf->Delay(numSamps);
			for (auto samp = 0u; samp < numSamps; samp++)
			{
				if (bufIter == buf->End())
					bufIter = buf->Start();

				outBuf[samp*numChannels + chan] = *bufIter++;
			}
		}
	}
}

unsigned int ChannelMixer::NumInputChannels() const
{
	return (unsigned int)_inputBuffers.size();
}

unsigned int ChannelMixer::NumOutputChannels() const
{
	return (unsigned int)_outputBuffers.size();
}

const std::shared_ptr<AudioSink> ChannelMixer::InputChannel(unsigned int channel)
{
	if (channel < _inputBuffers.size())
		return _inputBuffers[channel];

	return std::shared_ptr<AudioSink>();
}

const std::shared_ptr<AudioSource> ChannelMixer::OutputChannel(unsigned int channel)
{
	if (channel < _outputBuffers.size())
		return _outputBuffers[channel];

	return std::shared_ptr<AudioSource>();
}