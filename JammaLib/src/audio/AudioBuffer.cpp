#include "AudioBuffer.h"

using namespace audio;

AudioBuffer::AudioBuffer() :
	AudioSource({}),
	_index(0),
	_length(0),
	_buffer(std::vector<float>(0, 0.0f))
{
}

AudioBuffer::AudioBuffer(unsigned int size) :
	AudioSource({}),
	_index(0),
	_length(0),
	_buffer(std::vector<float>(size, 0.0f))
{
}

AudioBuffer::~AudioBuffer()
{
}

void AudioBuffer::OnPlay(const std::shared_ptr<base::AudioSink> dest,
	unsigned int numSamps)
{
	if (_length == 0)
		return;

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
}

void AudioBuffer::EndPlay(unsigned int numSamps)
{
	auto bufSize = (unsigned int)_buffer.size();
	_index += numSamps;

	while (bufSize <= _index)
		_index -= bufSize;

	_length += numSamps;
	if (_length > bufSize)
		_length = bufSize;
}

inline int AudioBuffer::OnWrite(float samp, int indexOffset)
{
	auto bufSize = (unsigned int)_buffer.size();

	while (bufSize <= _index + indexOffset)
		indexOffset -= (int)_buffer.size();

	_buffer[_index + indexOffset]+= samp;

	return indexOffset + 1;
}

inline int AudioBuffer::OnOverwrite(float samp, int indexOffset)
{
	auto bufSize = (unsigned int)_buffer.size();

	while (bufSize <= _index + indexOffset)
		indexOffset -= (int)_buffer.size();

	_buffer[_index + indexOffset] = samp;

	return indexOffset + 1;
}

void AudioBuffer::EndWrite(unsigned int numSamps, bool updateIndex)
{
	// Skip - EndPlay() will do the offset as
	// we are using the same buffers for IO
	//		SetIndex(_index + numSamps);

	_length += numSamps;
	if (_length > _buffer.size())
		_length = (unsigned int)_buffer.size();

	if (updateIndex)
		SetIndex(_index + numSamps);
}

void AudioBuffer::SetSize(unsigned int size)
{
	_buffer.resize(size);
}

void AudioBuffer::SetIndex(unsigned int index)
{
	auto bufSize = (unsigned int)_buffer.size();

	if (0 == bufSize)
	{
		_index = 0;
		return;
	}

	while (index >= bufSize)
		index-= bufSize;

	_index = index;
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
