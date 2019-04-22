#include "AudioMixer.h"

using namespace audio;
using gui::GuiSliderParams;

AudioMixer::AudioMixer() :
	_slider(GuiSliderParams()),
	_inputs({}),
	_outputs({})
{
}

AudioMixer::~AudioMixer()
{
}

void AudioMixer::Tick(unsigned int numSamps)
{

}