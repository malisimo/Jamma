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
		virtual void Play(std::shared_ptr<AudioSink> dest, unsigned int numSamps);
		virtual void Push(const float& samp);
		virtual void PushMix(const float& samp);
		
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
