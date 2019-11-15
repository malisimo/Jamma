#include "AudioBuffer.h"

using namespace audio;

AudioBuffer::AudioBuffer() :
	AudioSource({}),
	_playIndex(0),
	_length(0),
	_buffer(std::vector<float>(constants::MaxBlockSize, 0.0f))
{
}

AudioBuffer::AudioBuffer(unsigned int size) :
	AudioSource({}),
	_playIndex(0),
	_length(0),
	_buffer(std::vector<float>(size < constants::MaxBlockSize ? constants::MaxBlockSize : size, 0.0f))
{
}

AudioBuffer::~AudioBuffer()
{
}

void AudioBuffer::OnPlay(const std::shared_ptr<base::AudioSink> dest,
	unsigned int numSamps)
{
	if (0 == _length)
		return;

	auto index = _playIndex;
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
	_playIndex += numSamps;

	if (0 == _length)
	{
		_playIndex = 0;
		return;
	}

	if (_length < bufSize)
	{
		if (_playIndex > _writeIndex)
			_playIndex = _writeIndex;

		return;
	}

	while (bufSize <= _playIndex)
		_playIndex -= bufSize;
}

inline int AudioBuffer::OnWrite(float samp, int indexOffset)
{
	auto bufSize = (unsigned int)_buffer.size();

	if (0 == bufSize)
	{
		_writeIndex = 0;
		return 0;
	}

	while (bufSize <= _writeIndex + indexOffset)
		indexOffset -= (int)_buffer.size();

	_buffer[_writeIndex + indexOffset]+= samp;

	return indexOffset + 1;
}

inline int AudioBuffer::OnOverwrite(float samp, int indexOffset)
{
	auto bufSize = (unsigned int)_buffer.size();

	if (0 == bufSize)
	{
		_writeIndex = 0;
		return 0;
	}

	while (bufSize <= _writeIndex + indexOffset)
		indexOffset -= (int)_buffer.size();

	_buffer[_writeIndex + indexOffset] = samp;

	return indexOffset + 1;
}

void AudioBuffer::EndWrite(unsigned int numSamps, bool updateIndex)
{
	_length += numSamps;
	if (_length > _buffer.size())
		_length = (unsigned int)_buffer.size();

	if (updateIndex)
		_SetWriteIndex(_writeIndex + numSamps);
}

void AudioBuffer::SetSize(unsigned int size)
{
	_buffer.resize(size < constants::MaxBlockSize ? constants::MaxBlockSize : size);
}

void AudioBuffer::_SetWriteIndex(unsigned int index)
{
	auto bufSize = (unsigned int)_buffer.size();

	if (0 == bufSize)
	{
		_writeIndex = 0;
		return;
	}

	while (index >= bufSize)
		index-= bufSize;

	_writeIndex = index;
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
	if (0 == _length)
	{
		_playIndex = 0;
		return _buffer.begin();
	}

	auto sampsBehind = sampsDelay > _length ? _length : sampsDelay;
	_playIndex = sampsBehind > _writeIndex ? (_writeIndex + _length) - sampsBehind : _writeIndex - sampsBehind;
	return (_buffer.begin() + _playIndex);
}
