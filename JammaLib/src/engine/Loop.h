#pragma once

#include <string>
#include <memory>
#include "MultiAudioSource.h"
#include "ActionReceiver.h"
#include "ResourceUser.h"
#include "../audio/AudioMixer.h"
#include "../resources/WavResource.h"

namespace engine
{
	class LoopParams
	{
	public:
		std::string Wav;
	};

	class Loop :
		public base::MultiAudioSource,
		public base::ResourceUser,
		public base::ActionReceiver
	{
	public:
		Loop(LoopParams loopParams);
		Loop();
		~Loop() { ReleaseResources(); }

		// Copy
		Loop(const Loop&) = delete;
		Loop& operator=(const Loop&) = delete;

		// Move
		Loop(Loop&& other) :
			_index(other._index),
			_loopParams{other._loopParams},
			_mixer(std::move(other._mixer)),
			_wav(std::move(other._wav))
		{
			other._index = 0;
			other._loopParams = LoopParams();
			other._mixer = std::unique_ptr<audio::AudioMixer>();
			other._wav = std::weak_ptr<resources::WavResource>();
		}

		Loop& operator=(Loop&& other)
		{
			if (this != &other)
			{
				ReleaseResources();
				std::swap(_index, other._index);
				std::swap(_loopParams, other._loopParams);
				_mixer.swap(other._mixer);
				_wav.swap(other._wav);
			}

			return *this;
		}

	public:
		virtual bool InitResources(resources::ResourceLib& resourceLib) override;
		virtual bool ReleaseResources() override;
		virtual void Play(const std::vector<std::shared_ptr<base::AudioSink>>& dest, unsigned int numSamps) override;

	private:
		unsigned int _index;
		LoopParams _loopParams;
		std::unique_ptr<audio::AudioMixer> _mixer;
		std::weak_ptr<resources::WavResource> _wav;
	};
}
