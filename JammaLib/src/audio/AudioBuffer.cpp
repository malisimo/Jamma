#include "AudioBuffer.h"

using namespace audio;

AudioBuffer::AudioBuffer() :
	AudioSource({}),
	_buffer(std::vector<float>(0, 0.0f))
{
}

AudioBuffer::AudioBuffer(unsigned int size) :
	AudioSource({}),
	_buffer(std::vector<float>(size, 0.0f))
{
}

AudioBuffer::~AudioBuffer()
{
}

void AudioBuffer::Play(std::shared_ptr<AudioSink> buf, unsigned int numSamps)
{
	// TODO: AudioBuffer should output its samples when played...
}

void AudioBuffer::Zero(unsigned int numSamps)
{
	auto offset = 0;

	for (auto i = 0u; i < numSamps; i++)
		offset = Write(0.0f, offset);
}

void AudioBuffer::SetSize(unsigned int size)
{
	_buffer.resize(size);
}

void AudioBuffer::SetIndex(unsigned int index)
{
	auto bufSize = (unsigned int)_buffer.size();

	auto numExtraSamps = index - _index;
	_index = index;

	if (bufSize <= _index)
		_index = bufSize - 1;

	_numSamps += numExtraSamps;
	if (_numSamps > bufSize)
		_numSamps = bufSize;
}

unsigned int AudioBuffer::NumSamps() const
{
	return _numSamps;
}

unsigned int AudioBuffer::BufSize() const
{
	return (unsigned int)_buffer.size();
}

std::vector<float>::iterator AudioBuffer::Start()
{
	return _buffer.begin();
}

std::vector<float>::iterator AudioBuffer::End()
{
	return _buffer.end();
}

std::vector<float>::iterator AudioBuffer::Delay(unsigned int sampsDelay)
{
	auto bufSize = (unsigned int)_buffer.size();

	if (0 == bufSize)
		return _buffer.begin();

	auto sampsBehind = sampsDelay >= bufSize ? bufSize - 1 : sampsDelay;
	auto offset = sampsBehind > _index ? (_index + bufSize) - sampsBehind : _index - sampsBehind;
	return (_buffer.begin() + offset);
}
