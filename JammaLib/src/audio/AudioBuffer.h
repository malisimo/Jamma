#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include "AudioSource.h"

namespace audio
{
	class AudioBuffer :
		public base::AudioSink,
		public base::AudioSource
	{
	public:
		AudioBuffer();
		AudioBuffer(unsigned int size);
		~AudioBuffer();

	public:
		virtual void Play(std::shared_ptr<AudioSink> dest, unsigned int numSamps) override;
		inline virtual int Write(float samp, int indexOffset) override
		{
			auto bufSize = (unsigned int)_buffer.size();

			while (bufSize <= _index + indexOffset)
				indexOffset -= (int)_buffer.size();

			_buffer[_index + indexOffset] = samp;

			indexOffset++;

			_numSamps++;
			if (_numSamps > bufSize)
				_numSamps = bufSize;

			return indexOffset;
		}

		inline virtual int WriteMix(float samp, int indexOffset) override
		{
			auto bufSize = (unsigned int)_buffer.size();

			while (bufSize <= _index + indexOffset)
				indexOffset -= (int)_buffer.size();

			_buffer[_index + indexOffset] += samp;

			indexOffset++;

			_numSamps++;
			if (_numSamps > bufSize)
				_numSamps = bufSize;

			return indexOffset;
		}

		inline virtual void Offset(int indexOffset) override
		{
			auto bufSize = (unsigned int)_buffer.size();
			_index += indexOffset;

			while (bufSize <= _index)
				_index -= bufSize;

			_numSamps += indexOffset > 0 ? indexOffset : bufSize;
			if (_numSamps > bufSize)
				_numSamps = bufSize;
		}

		void Zero(unsigned int numSamps);
		void SetSize(unsigned int size);
		void SetIndex(unsigned int index);
		unsigned int NumSamps() const;
		unsigned int BufSize() const;

		std::vector<float>::iterator Start();
		std::vector<float>::iterator End();
		std::vector<float>::iterator Delay(unsigned int sampsDelay);

	protected:
		unsigned int _index;
		unsigned int _numSamps;
		std::vector<float> _buffer;
	};
}
