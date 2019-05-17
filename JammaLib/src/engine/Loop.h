#pragma once

#include <string>
#include <memory>
#include "MultiAudioSource.h"
#include "ActionReceiver.h"
#include "ResourceUser.h"
#include "GuiElement.h"
#include "../audio/AudioMixer.h"
#include "../resources/WavResource.h"

namespace engine
{
	class LoopParams :
		public base::GuiElementParams
	{
	public:
		LoopParams() :
			base::GuiElementParams(DrawableParams{ "" },
				MoveableParams{ 0,0 },
				SizeableParams{ 1,1 },
				"",
				"",
				"",
				{}),
			Wav(""),
			MixerParams(audio::AudioMixerParams())
		{
		}

		LoopParams(base::GuiElementParams params,
			std::string wav,
			audio::AudioMixerParams mixerParams) :
			base::GuiElementParams(params),
			Wav(wav),
			MixerParams(mixerParams)
		{
		}

	public:
		std::string Wav;
		audio::AudioMixerParams MixerParams;
	};

	class Loop :
		public base::MultiAudioSource,
		public base::GuiElement
	{
	public:
		Loop(LoopParams loopParams);
		~Loop() { ReleaseResources(); }

		// Copy
		Loop(const Loop&) = delete;
		Loop& operator=(const Loop&) = delete;

		// Move
		Loop(Loop&& other) :
			GuiElement(other._guiParams),
			_index(other._index),
			_loopParams{other._loopParams},
			_mixer(std::move(other._mixer)),
			_wav(std::move(other._wav))
		{
			other._index = 0;
			other._loopParams = LoopParams();
			other._mixer = std::make_unique<audio::AudioMixer>(audio::AudioMixerParams());
			other._wav = std::weak_ptr<resources::WavResource>();
		}

		Loop& operator=(Loop&& other)
		{
			if (this != &other)
			{
				ReleaseResources();
				std::swap(_guiParams, other._guiParams),
				std::swap(_index, other._index);
				std::swap(_loopParams, other._loopParams);
				_mixer.swap(other._mixer);
				_wav.swap(other._wav);
			}

			return *this;
		}

	public:
		virtual void Play(const std::vector<std::shared_ptr<base::AudioSink>>& dest, unsigned int numSamps) override;

	protected:
		virtual bool _InitResources(resources::ResourceLib& resourceLib) override;
		virtual bool _ReleaseResources() override;

	private:
		unsigned int _index;
		LoopParams _loopParams;
		std::shared_ptr<audio::AudioMixer> _mixer;
		std::weak_ptr<resources::WavResource> _wav;
	};
}
