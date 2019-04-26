#include "AudioMixer.h"

using namespace audio;
using base::AudioSink;
using gui::GuiSliderParams;

AudioMixer::AudioMixer() :
	_behaviour(MixBehaviour()),
	_slider(GuiSliderParams())
{
}

AudioMixer::~AudioMixer()
{
}

void AudioMixer::Play(const std::vector<std::shared_ptr<AudioSink>>& dest, float samp, bool mix)
{
	_behaviour.Apply(dest, samp, mix);
}

void WireMixBehaviour::Apply(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, bool mix)
{
	std::function<void(std::shared_ptr<base::AudioSink>, float)> mixFun = [](std::shared_ptr<base::AudioSink> buf, float s) { buf->PushMix(s);	};
	std::function<void(std::shared_ptr<base::AudioSink>, float)> replaceFun = [](std::shared_ptr<base::AudioSink> buf, float s) { buf->Push(s);		};
	auto pushFun = mix ? mixFun : replaceFun;

	unsigned int chan = 0;
	for (auto buf : dest)
	{
		if (std::find(_channels.begin(), _channels.end(), chan) != _channels.end())
			pushFun(buf, samp);

		chan++;
	}
}

void PanMixBehaviour::Apply(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, bool mix)
{
	std::function<void(std::shared_ptr<base::AudioSink>, float)> mixFun = [](std::shared_ptr<base::AudioSink> buf, float s) { buf->PushMix(s);	};
	std::function<void(std::shared_ptr<base::AudioSink>, float)> replaceFun = [](std::shared_ptr<base::AudioSink> buf, float s) { buf->Push(s);		};
	auto pushFun = mix ? mixFun : replaceFun;

	unsigned int chan = 0;
	for (auto buf : dest)
	{
		if (_channelLevels.size() > chan)
		{
			if (_channelLevels[chan] > 0.f)
				pushFun(buf, samp * _channelLevels.at(chan));
		}
		chan++;
	}
}
