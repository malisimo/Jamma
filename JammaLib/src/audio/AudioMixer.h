#pragma once

#include <vector>
#include <memory>
#include "AudioSource.h"
#include "AudioSink.h"
#include "GuiElement.h"
#include "../gui/GuiSlider.h"

namespace audio
{
	class MixBehaviour
	{
	public:
		virtual void Apply(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, unsigned int index) const {}
	};

	class WireMixBehaviour : public MixBehaviour
	{
	public:
		virtual void Apply(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, unsigned int index) const override;

		std::vector<unsigned int> Channels;
	};

	class PanMixBehaviour : public MixBehaviour
	{
	public:
		virtual void Apply(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, unsigned int index) const override;

		std::vector<float> ChannelLevels;
	};

	class AudioMixerParams :
		public base::GuiElementParams
	{
	public:
		AudioMixerParams() :
			base::GuiElementParams(DrawableParams{ "" },
				MoveableParams{ 0,0 },
				SizeableParams{ 1,1 },
				"",
				"",
				"",
				{}),
			Behaviour(MixBehaviour())
		{
		}

		AudioMixerParams(base::GuiElementParams params,
			MixBehaviour behaviour) :
			base::GuiElementParams(params),
			Behaviour(behaviour)
		{
		}

	public:
		MixBehaviour Behaviour;
	};

	class AudioMixer :
		public base::GuiElement
	{
	public:
		AudioMixer(AudioMixerParams params);

	public:
		void SetBehaviour(std::unique_ptr<MixBehaviour> behaviour);
		void Play(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, unsigned int index);
		void Offset(const std::vector<std::shared_ptr<base::AudioSink>>& dest, unsigned int index);

	protected:
		std::unique_ptr<MixBehaviour> _behaviour;
		gui::GuiSlider _slider;
	};
}
