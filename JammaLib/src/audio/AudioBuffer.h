#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include "Audible.h"

namespace audio
{
	class AudioBuffer : public base::Audible
	{
	public:
		AudioBuffer();
		AudioBuffer(unsigned int size);
		~AudioBuffer();

	public:
		virtual void Play(std::shared_ptr<AudioBuffer> buf, unsigned int numSamps) override;

		void SetSize(unsigned int size);
		void SetIndex(unsigned int index);
		unsigned int NumSamps() const;
		unsigned int BufSize() const;
		void Push(const float& samp);
		void PushMix(const float& samp);

		std::vector<float>::iterator Start();
		std::vector<float>::iterator End();
		std::vector<float>::iterator Delay(unsigned int sampsDelay);

	protected:
		unsigned int _index;
		unsigned int _numSamps;
		std::vector<float> _buffer;
	};
}
