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

void AudioBuffer::SetSize(unsigned int size)
{
	_buffer.resize(size);
}

void AudioBuffer::SetIndex(unsigned int index)
{
	_index = index % _buffer.size();
}

unsigned int AudioBuffer::NumSamps() const
{
	return _numSamps;
}

unsigned int AudioBuffer::BufSize() const
{
	return (unsigned int)_buffer.size();
}

void AudioBuffer::Push(const float &samp)
{
	auto bufSize = (unsigned int)_buffer.size(); 

	if (_index < bufSize)
		_buffer[_index++] = samp;

	if (_index >= bufSize)
		_index -= (unsigned int)_buffer.size();

	_numSamps++;
	if (_numSamps > bufSize)
		_numSamps = bufSize;
}

void AudioBuffer::PushMix(const float &samp)
{
	auto bufSize = (unsigned int)_buffer.size();

	if (_index < bufSize)
		_buffer[_index++]+= samp;

	if (_index >= bufSize)
		_index -= (unsigned int)_buffer.size();

	_numSamps++;
	if (_numSamps > bufSize)
		_numSamps = bufSize;
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

