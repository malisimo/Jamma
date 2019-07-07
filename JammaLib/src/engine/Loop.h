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
		std::string PlayTexture;
		std::string RecordTexture;
		std::string OverdubTexture;
		std::string PunchTexture;
		audio::AudioMixerParams MixerParams;
	};

	class Loop :
		public base::AudioSink,
		public base::MultiAudioSource,
		public base::GuiElement
	{
	public:
		enum LoopVisualState
		{
			STATE_INACTIVE,
			STATE_PLAYING,
			STATE_RECORDING,
			STATE_OVERDUBBING,
			STATE_PUNCHEDIN
		};

	public:
		Loop(LoopParams loopParams);
		~Loop() { ReleaseResources(); }

		// Copy
		Loop(const Loop&) = delete;
		Loop& operator=(const Loop&) = delete;

		// Move
		Loop(Loop&& other) :
			GuiElement(other._guiParams),
			_playPos(other._playPos),
			_recPos(other._recPos),
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
		virtual void OnPlay(const std::shared_ptr<base::MultiAudioSink> dest, unsigned int numSamps) override;
		virtual int OnWrite(float samp, int indexOffset) override;
		//virtual int WriteMix(float samp, int indexOffset) override;
		//virtual void Offset(int indexOffset) override;

		unsigned int InputChannel();
		void SetInputChannel(unsigned int channel);

		void Record();
		void Play(unsigned long index, unsigned long length);
		void Ditch();
		void Overdub();
		void PunchIn();
		void PunchOut();

	protected:
		virtual bool _InitResources(resources::ResourceLib& resourceLib) override;
		virtual bool _ReleaseResources() override;

	private:
		static const unsigned int _MaxFadeSamps = 30000;
		static const unsigned int _InitBufferSize = 1000000;
		static const unsigned int _MaxBufferSize = 40000000;

		unsigned long _playPos;
		unsigned long _recPos;
		double _pitch;
		unsigned long _length;
		LoopVisualState _state;
		LoopParams _loopParams;
		std::shared_ptr<audio::AudioMixer> _mixer;
		std::weak_ptr<resources::WavResource> _wav;
		std::vector<float> _buffer;
	};
}
