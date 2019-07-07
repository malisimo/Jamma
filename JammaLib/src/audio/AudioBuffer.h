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
		virtual AudioDirection AudibleDirection() const override
		{
			return AUDIO_BOTH;
		}
		virtual void OnPlay(const std::shared_ptr<base::AudioSink> dest,
			unsigned int numSamps) override;
		inline virtual int OnWrite(float samp, int indexOffset) override;
		inline virtual void Offset(int indexOffset) override;

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
		unsigned int _length;
		std::vector<float> _buffer;
	};
}
