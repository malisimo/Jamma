#pragma once

#include <vector>
#include <memory>
#include "Audible.h"
#include "AudioBuffer.h"
#include "../gui/GuiSlider.h"

namespace audio
{
	class AudioMixer
	{
	public:
		AudioMixer();
		~AudioMixer();

	public:
		void Tick(unsigned int numSamps);

	protected:
		gui::GuiSlider _slider;
		std::vector<std::shared_ptr<base::Audible>> _inputs;
		std::vector<std::shared_ptr<AudioBuffer>> _outputs;
	};
}
