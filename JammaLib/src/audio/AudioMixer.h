#pragma once

#include <vector>
#include <memory>
#include "AudioSource.h"
#include "AudioSink.h"
#include "../gui/GuiSlider.h"

namespace audio
{
	class MixBehaviour
	{
	public:
		virtual void Apply(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, bool mix) {}
	};

	class WireMixBehaviour : public MixBehaviour
	{
	public:
		virtual void Apply(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, bool mix) override;

		std::vector<unsigned int> _channels;
	};

	class PanMixBehaviour : public MixBehaviour
	{
	public:
		virtual void Apply(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, bool mix) override;

		std::vector<float> _channelLevels;
	};

	class AudioMixer
	{
	public:
		AudioMixer();
		~AudioMixer();

	public:
		void Play(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, bool mix);

	protected:
		MixBehaviour _behaviour;
		gui::GuiSlider _slider;
	};
}
