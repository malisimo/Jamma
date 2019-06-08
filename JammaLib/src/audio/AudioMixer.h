#pragma once

#include <vector>
#include <memory>
#include <variant>
#include "AudioSource.h"
#include "AudioSink.h"
#include "InterpolatedValue.h"
#include "GuiElement.h"
#include "../actions/DoubleAction.h"
#include "../gui/GuiSlider.h"

namespace audio
{
	class MixBehaviour
	{
	public:
		virtual void Apply(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, unsigned int index) const {}
	};

	class MixBehaviourParams {};

	class WireMixBehaviourParams : public MixBehaviourParams
	{
	public:
		std::vector<unsigned int> Channels;
	};

	class WireMixBehaviour : public MixBehaviour
	{
	public:
		WireMixBehaviour(WireMixBehaviourParams mixParams) :
			MixBehaviour()
		{
			_mixParams = mixParams;
		}

	public:
		virtual void Apply(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, unsigned int index) const override;

	protected:
		WireMixBehaviourParams _mixParams;
	};

	class PanMixBehaviourParams : public MixBehaviourParams
	{
	public:
		std::vector<float> ChannelLevels;
	};

	class PanMixBehaviour : public MixBehaviour
	{
	public:
		PanMixBehaviour(PanMixBehaviourParams mixParams) :
			MixBehaviour()
		{
			_mixParams = mixParams;
		}

	public:
		virtual void Apply(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, unsigned int index) const override;

	protected:
		PanMixBehaviourParams _mixParams;
	};

	typedef std::variant<MixBehaviourParams, PanMixBehaviourParams, WireMixBehaviourParams> BehaviourParams;

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
			Behaviour(MixBehaviourParams()),
			SliderParams(gui::GuiSliderParams())
		{
		}

		AudioMixerParams(base::GuiElementParams params,
			BehaviourParams behaviour,
			gui::GuiSliderParams sliderParams) :
			base::GuiElementParams(params),
			Behaviour(behaviour),
			SliderParams(sliderParams)
		{
		}

	public:
		BehaviourParams Behaviour;
		gui::GuiSliderParams SliderParams;
	};
	
	struct MixerBehaviourFactory
	{
		std::unique_ptr<MixBehaviour> operator()(MixBehaviourParams mixParams) const {
			return std::move(std::unique_ptr<MixBehaviour>());
		}
		std::unique_ptr<MixBehaviour> operator()(PanMixBehaviourParams panParams) const {
			return std::move(std::make_unique<PanMixBehaviour>(panParams));
		}
		std::unique_ptr<MixBehaviour> operator()(WireMixBehaviourParams wireParams) const {
			return std::move(std::make_unique<WireMixBehaviour>(wireParams));
		}
	};

	class AudioMixer :
		public base::GuiElement
	{
	public:
		AudioMixer(AudioMixerParams params);

	public:
		virtual actions::ActionResult OnAction(actions::DoubleAction val) override;
		virtual void InitReceivers() override;

		void Play(const std::vector<std::shared_ptr<base::AudioSink>>& dest, float samp, unsigned int index);
		void Offset(const std::vector<std::shared_ptr<base::AudioSink>>& dest, unsigned int index);

	protected:
		std::unique_ptr<MixBehaviour> _behaviour;
		std::shared_ptr<gui::GuiSlider> _slider;
		std::unique_ptr<InterpolatedValue> _fade;
	};
}
