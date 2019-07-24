#include "AudioMixer.h"

using namespace audio;
using namespace actions;
using base::AudioSource;
using base::MultiAudioSink;
using base::GuiElement;
using gui::GuiSlider;
using gui::GuiSliderParams;

AudioMixer::AudioMixer(AudioMixerParams params) :
	GuiElement(params),
	_inputChannel(params.InputChannel),
	_behaviour(std::unique_ptr<MixBehaviour>()),
	_slider(std::make_shared<GuiSlider>(params.SliderParams)),
	_fade(std::make_unique<InterpolatedValueExp>())
{
	_behaviour = std::visit(MixerBehaviourFactory{}, params.Behaviour);

	InterpolatedValueExp::ExponentialParams interpParams;
	interpParams.Damping = 100.0f;

	_fade = std::make_unique<InterpolatedValueExp>(interpParams);
	_fade->SetTarget(1.0);

	_children.push_back(_slider);
}

void AudioMixer::InitReceivers()
{
	_slider->SetReceiver(ActionReceiver::shared_from_this());
	_slider->SetValue(0.2);
}

ActionResult AudioMixer::OnAction(DoubleAction val)
{
	_fade->SetTarget(val.Value());
	return { true, 0UL, ACTIONRESULT_DEFAULT, nullptr };
}

void AudioMixer::OnPlay(const std::shared_ptr<MultiAudioSink> dest,
	float samp,
	unsigned int index)
{
	_behaviour->Apply(dest, samp * (float)_fade->Next(), index);
}

void AudioMixer::Offset(unsigned int numSamps)
{
	// TODO: Update fader state
}

unsigned int AudioMixer::InputChannel() const
{
	return _inputChannel;
}

void AudioMixer::SetInputChannel(unsigned int channel)
{
	_inputChannel = channel;
}

void WireMixBehaviour::Apply(const std::shared_ptr<MultiAudioSink> dest,
	float samp,
	unsigned int index) const
{
	auto numChans = dest->NumInputChannels();

	for (auto chan = 0u; chan < numChans; chan++)
	{
		if (std::find(_mixParams.Channels.begin(), _mixParams.Channels.end(), chan) != _mixParams.Channels.end())
			dest->OnWriteChannel(chan, samp, index);
	}
}

void PanMixBehaviour::Apply(const std::shared_ptr<MultiAudioSink> dest,
	float samp,
	unsigned int index) const
{
	auto numChans = dest->NumInputChannels();

	for (auto chan = 0u; chan < numChans; chan++)
	{
		if (chan < _mixParams.ChannelLevels.size())
			dest->OnWriteChannel(chan, samp * _mixParams.ChannelLevels.at(chan), index);
	}
}