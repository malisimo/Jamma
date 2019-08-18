#pragma once

#include <string>
#include <memory>
#include "MultiAudioSource.h"
#include "ActionReceiver.h"
#include "ResourceUser.h"
#include "GuiElement.h"
#include "../io/FileReadWriter.h"
#include "../io/JamFile.h"
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
			Wav("")
		{
		}

		LoopParams(base::GuiElementParams params,
			std::string wav) :
			base::GuiElementParams(params),
			Wav(wav)
		{
		}

	public:
		std::string Wav;
		std::string PlayTexture;
		std::string RecordTexture;
		std::string OverdubTexture;
		std::string PunchTexture;
	};

	class Loop :
		public virtual base::GuiElement,
		public virtual base::AudioSink,
		public virtual base::MultiAudioSource
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
		Loop(LoopParams loopParams,
			audio::AudioMixerParams mixerParams);
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
			_mixer(std::move(other._mixer))
		{
			other._index = 0;
			other._loopParams = LoopParams();
			other._mixer = std::make_unique<audio::AudioMixer>(audio::AudioMixerParams());
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
			}

			return *this;
		}

	public:
		static std::optional<std::shared_ptr<Loop>> FromFile(LoopParams loopParams, io::JamFile::Loop loopStruct, std::wstring dir);
		static audio::AudioMixerParams GetMixerParams(utils::Size2d loopSize, audio::BehaviourParams behaviour);

		virtual MultiAudioDirection MultiAudibleDirection() const override { return MULTIAUDIO_BOTH; }
		virtual void OnPlay(const std::shared_ptr<base::MultiAudioSink> dest, unsigned int numSamps) override;
		virtual void EndMultiPlay(unsigned int numSamps) override;
		inline virtual int OnWrite(float samp, int indexOffset) override;
		inline virtual int OnOverwrite(float samp, int indexOffset) override;
		virtual void EndWrite(unsigned int numSamps, bool updateIndex) override;
		void OnPlayRaw(const std::shared_ptr<base::MultiAudioSink> dest, unsigned int channel, unsigned int delaySamps, unsigned int numSamps);

		unsigned int InputChannel();
		void SetInputChannel(unsigned int channel);

		bool Load(const io::WavReadWriter& readWriter);
		void Record();
		void Play(unsigned long index, unsigned long length);
		void Ditch();
		void Overdub();
		void PunchIn();
		void PunchOut();

	protected:
		static const unsigned int _MaxFadeSamps = 30000;
		static const unsigned int _InitBufferSize = 1000000;
		static const unsigned int _MaxBufferSize = 40000000;
		static const utils::Size2d _Gap;
		static const utils::Size2d _DragGap;
		static const utils::Size2d _DragSize;

		unsigned long _playPos;
		unsigned long _recPos;
		double _pitch;
		unsigned long _length;
		LoopVisualState _state;
		LoopParams _loopParams;
		std::shared_ptr<audio::AudioMixer> _mixer;
		std::vector<float> _buffer;
	};
}
