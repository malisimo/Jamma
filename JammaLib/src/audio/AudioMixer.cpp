#include "AudioMixer.h"

using namespace audio;
using base::AudioSink;
using base::GuiElement;
using gui::GuiSliderParams;

AudioMixer::AudioMixer(AudioMixerParams params) :
	GuiElement(params),
	_behaviour(std::unique_ptr<MixBehaviour>()),
	_slider(GuiSliderParams())
{
}

void AudioMixer::SetBehaviour(std::unique_ptr<MixBehaviour> behaviour)
{
	_behaviour = std::move(behaviour);
}


void AudioMixer::Play(const std::vector<std::shared_ptr<AudioSink>>& dest, float samp, unsigned int index)
{
	_behaviour->Apply(dest, samp, index);
}

void AudioMixer::Offset(const std::vector<std::shared_ptr<base::AudioSink>>& dest, unsigned int index)
{
	for (auto& buf : dest)
		buf->Offset(index);
		
}

void WireMixBehaviour::Apply(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, unsigned int offset) const
{
	unsigned int chan = 0;
	for (auto& buf : dest)
	{
		if (std::find(Channels.begin(), Channels.end(), chan) != Channels.end())
			buf->WriteMix(samp, offset);

		chan++;
	}
}

void PanMixBehaviour::Apply(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, unsigned int offset) const
{
	unsigned int chan = 0;
	for (auto& buf : dest)
	{
		if (chan < ChannelLevels.size())
			buf->WriteMix(samp * ChannelLevels.at(chan), offset);

		chan++;
	}
}