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
		virtual void Apply(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, unsigned int index) {}
	};

	class WireMixBehaviour : public MixBehaviour
	{
	public:
		virtual void Apply(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, unsigned int index) override;

		std::vector<unsigned int> Channels;
	};

	class PanMixBehaviour : public MixBehaviour
	{
	public:
		virtual void Apply(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, unsigned int index) override;

		std::vector<float> ChannelLevels;
	};

	class AudioMixer
	{
	public:
		AudioMixer();
		~AudioMixer();

	public:
		void SetBehaviour(std::unique_ptr<MixBehaviour> behaviour);
		void Play(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, unsigned int index);
		void Offset(const std::vector<std::shared_ptr<base::AudioSink>>& dest, unsigned int index);

	protected:
		std::unique_ptr<MixBehaviour> _behaviour;
		gui::GuiSlider _slider;
	};
}
