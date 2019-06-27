#include "AudioMixer.h"

using namespace audio;
using namespace actions;
using base::AudioSink;
using base::GuiElement;
using gui::GuiSlider;
using gui::GuiSliderParams;

AudioMixer::AudioMixer(AudioMixerParams params) :
	GuiElement(params),
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
	return { true, ACTIONRESULT_DEFAULT, nullptr };
}

void AudioMixer::Play(const std::vector<std::shared_ptr<AudioSink>>& dest, float samp, unsigned int index)
{
	_behaviour->Apply(dest, samp * (float)_fade->Next(), index);
}

void AudioMixer::Offset(const std::vector<std::shared_ptr<base::AudioSink>>& dest, unsigned int index)
{
	for (auto& buf : dest)
		buf->Offset(index);
}

void WireMixBehaviour::Apply(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, unsigned int offset) const
{
	auto chan = 0u;
	for (auto& buf : dest)
	{
		if (std::find(_mixParams.Channels.begin(), _mixParams.Channels.end(), chan) != _mixParams.Channels.end())
		{
			buf->WriteMix(samp, offset);
		}

		chan++;
	}
}

void PanMixBehaviour::Apply(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, unsigned int offset) const
{
	auto chan = 0u;
	for (auto& buf : dest)
	{
		if (chan < _mixParams.ChannelLevels.size())
			buf->WriteMix(samp * _mixParams.ChannelLevels.at(chan), offset);

		chan++;
	}
}