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

void AudioBuffer::OnPlay(const std::shared_ptr<base::AudioSink> dest,
	unsigned int numSamps)
{
	auto index = _index;
	while (index >= _length)
		index -= _length;

	auto destIndex = 0;

	for (auto i = 0u; i < numSamps; i++)
	{
		destIndex = dest->OnWrite(_buffer[index], destIndex);

		index++;
		if (index >= _length)
			index -= _length;
	}

	_index += numSamps;
	_index %= _length;
}

void AudioBuffer::Zero(unsigned int numSamps)
{
	auto offset = 0;

	for (auto i = 0u; i < numSamps; i++)
		offset = OnWrite(0.0f, offset);
}

inline int AudioBuffer::OnWrite(float samp, int indexOffset)
{
	auto bufSize = (unsigned int)_buffer.size();

	while (bufSize <= _index + indexOffset)
		indexOffset -= (int)_buffer.size();

	_buffer[_index + indexOffset]+= samp;

	indexOffset++;

	_length++;
	if (_length > bufSize)
		_length = bufSize;

	return indexOffset;
}

inline void AudioBuffer::Offset(int indexOffset)
{
	auto bufSize = (unsigned int)_buffer.size();
	_index += indexOffset;

	while (bufSize <= _index)
		_index -= bufSize;

	_length += indexOffset > 0 ? indexOffset : bufSize;
	if (_length > bufSize)
		_length = bufSize;
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

	_length += numExtraSamps;
	if (_length > bufSize)
		_length = bufSize;
}

unsigned int AudioBuffer::NumSamps() const
{
	return _length;
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
