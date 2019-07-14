#include "ChannelMixer.h"

using namespace audio;
using namespace base;

ChannelMixer::ChannelMixer(ChannelMixerParams chanMixParams) :
	_adcMixer(std::make_shared<ChannelMixer::AdcChannelMixer>()),
	_dacMixer(std::make_shared<ChannelMixer::DacChannelMixer>())
{
	SetParams(chanMixParams);
}

ChannelMixer::~ChannelMixer()
{
}

void ChannelMixer::SetParams(ChannelMixerParams chanMixParams)
{
	_adcMixer->SetNumChannels(chanMixParams.NumInputChannels, chanMixParams.InputBufferSize);
	_dacMixer->SetNumChannels(chanMixParams.NumOutputChannels, chanMixParams.OutputBufferSize);
}

void ChannelMixer::FromAdc(float* inBuf, unsigned int numChannels, unsigned int numSamps)
{
	if (numSamps < 1 || numChannels < 1)
		return;

	for (auto chan = 0u; chan < _adcMixer->NumOutputChannels(); chan++)
	{
		auto buf = _adcMixer->Channel(chan);

		if ((buf) && (numChannels > chan))
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

	for (auto chan = 0u; chan < _dacMixer->NumInputChannels(); chan++)
	{
		auto buf = _dacMixer->Channel(chan);

		if (buf)
		{
			if ((numChannels > chan) && (buf->BufSize() > 0))
			{
				auto bufIter = buf->Delay(numSamps);
				for (auto samp = 0u; samp < numSamps; samp++)
				{
					if (bufIter == buf->End())
						bufIter = buf->Start();

					outBuf[samp * numChannels + chan] = *bufIter++;
				}
			}
		}
	}
}

void ChannelMixer::Offset(unsigned int numSamps)
{
	_dacMixer->Offset(numSamps);
}

void ChannelMixer::BufferMixer::SetNumChannels(unsigned int numChans, unsigned int bufSize)
{
	auto numInputs = (unsigned int)_buffers.size();

	if (numChans > numInputs)
	{
		for (auto i = 0u; i < numChans - numInputs; i++)
			_buffers.push_back(std::make_unique<AudioBuffer>(bufSize));
	}

	if (numChans < numInputs)
		_buffers.resize(numInputs);

	for (auto& buf : _buffers)
		buf->SetSize(bufSize);
}

unsigned int ChannelMixer::DacChannelMixer::NumInputChannels() const
{
	return (unsigned int)_buffers.size();
}

unsigned int ChannelMixer::AdcChannelMixer::NumOutputChannels() const
{
	return (unsigned int)_buffers.size();
}

const std::shared_ptr<AudioBuffer> ChannelMixer::BufferMixer::Channel(unsigned int channel)
{
	if (channel < _buffers.size())
		return _buffers[channel];

	return std::shared_ptr<AudioBuffer>();
}

const std::shared_ptr<AudioSink> ChannelMixer::DacChannelMixer::InputChannel(unsigned int channel)
{
	if (channel < _buffers.size())
		return _buffers[channel];

	return std::shared_ptr<AudioSink>();
}

const std::shared_ptr<AudioSource> ChannelMixer::AdcChannelMixer::OutputChannel(unsigned int channel)
{
	if (channel < _buffers.size())
		return _buffers[channel];

	return std::shared_ptr<AudioSource>();
}

const std::shared_ptr<MultiAudioSource> ChannelMixer::Source()
{
	return _adcMixer;
}

const std::shared_ptr<MultiAudioSink> ChannelMixer::Sink()
{
	return _dacMixer;
}