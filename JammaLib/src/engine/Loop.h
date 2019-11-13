#pragma once

#include <string>
#include <memory>
#include "MultiAudioSource.h"
#include "ActionReceiver.h"
#include "ResourceUser.h"
#include "GuiElement.h"
#include "GlUtils.h"
#include "VU.h"
#include "LoopModel.h"
#include "../gui/GuiModel.h"
#include "../io/FileReadWriter.h"
#include "../io/JamFile.h"
#include "../audio/AudioMixer.h"
#include "../graphics/GlDrawContext.h"
#include "../resources/WavResource.h"

namespace engine
{
	class LoopParams :
		public base::GuiElementParams
	{
	public:
		LoopParams() :
			base::GuiElementParams(DrawableParams{ "" },
				MoveableParams(utils::Position2d{ 0, 0 }, utils::Position3d{ 0, 0, 0 }, 1.0),
				SizeableParams{ 1,1 },
				"",
				"",
				"",
				{}),
			Id(""),
			TakeId(""),
			Wav(""),
			PlayTexture(""),
			RecordTexture(""),
			OverdubTexture(""),
			PunchTexture(""),
			FadeSamps(800u)
		{
		}

		LoopParams(base::GuiElementParams params,
			std::string wav) :
			base::GuiElementParams(params),
			Id(""),
			TakeId(""),
			Wav(wav),
			PlayTexture(""),
			RecordTexture(""),
			OverdubTexture(""),
			PunchTexture(""),
			FadeSamps(800u)
		{
		}

	public:
		std::string Id;
		std::string TakeId;
		std::string Wav;
		std::string PlayTexture;
		std::string RecordTexture;
		std::string OverdubTexture;
		std::string PunchTexture;
		unsigned int FadeSamps;
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
			STATE_RECORDING,
			STATE_PLAYINGRECORDING,
			STATE_PLAYING,
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
			_modelNeedsUpdating(other._modelNeedsUpdating),
			_endRecordingCompleted(other._endRecordingCompleted),
			_lastPeak(other._lastPeak),
			_pitch(other._pitch),
			_loopLength(other._loopLength),
			_state(other._state),
			_playIndex(other._playIndex),
			_loopParams{other._loopParams},
			_mixer(std::move(other._mixer)),
			_model(std::move(other._model)),
			_vu(std::move(other._vu)),
			_buffer(std::move(other._buffer)),
			_backBuffer(std::move(other._backBuffer))
		{
			other._writeIndex = 0;
			other._loopParams = LoopParams();
			other._mixer = std::make_unique<audio::AudioMixer>(audio::AudioMixerParams());
		}

		Loop& operator=(Loop&& other)
		{
			if (this != &other)
			{
				ReleaseResources();
				std::swap(_modelNeedsUpdating, other._modelNeedsUpdating);
				std::swap(_endRecordingCompleted, other._endRecordingCompleted);
				std::swap(_lastPeak, other._lastPeak);
				std::swap(_pitch, other._pitch);
				std::swap(_loopLength, other._loopLength);
				std::swap(_state, other._state);
				std::swap(_guiParams, other._guiParams);
				std::swap(_writeIndex, other._writeIndex);
				std::swap(_playIndex, other._playIndex);
				std::swap(_loopParams, other._loopParams);
				_mixer.swap(other._mixer);
				_model.swap(other._model);
				_vu.swap(other._vu);
				_buffer.swap(other._buffer);
				_backBuffer.swap(other._backBuffer);
			}

			return *this;
		}

	public:
		static std::optional<std::shared_ptr<Loop>> FromFile(LoopParams loopParams,
			io::JamFile::Loop loopStruct,
			std::wstring dir);
		static audio::AudioMixerParams GetMixerParams(utils::Size2d loopSize,
			audio::BehaviourParams behaviour);

		virtual utils::Position2d Position() const override;
		virtual void SetSize(utils::Size2d size) override;
		virtual MultiAudioDirection MultiAudibleDirection() const override { return MULTIAUDIO_BOTH; }
		virtual void Draw3d(base::DrawContext& ctx) override;
		virtual void OnPlay(const std::shared_ptr<base::MultiAudioSink> dest, unsigned int numSamps) override;
		virtual void EndMultiPlay(unsigned int numSamps) override;
		inline virtual int OnWrite(float samp, int indexOffset) override;
		inline virtual int OnOverwrite(float samp, int indexOffset) override;
		virtual void EndWrite(unsigned int numSamps, bool updateIndex) override;
		virtual actions::ActionResult OnAction(actions::JobAction action) override;

		void OnPlayRaw(const std::shared_ptr<base::MultiAudioSink> dest,
			unsigned int channel,
			unsigned int delaySamps,
			unsigned int numSamps);
		unsigned int InputChannel() const;
		void SetInputChannel(unsigned int channel);
		std::string Id() const;

		bool Load(const io::WavReadWriter& readWriter);
		void Record();
		void Play(unsigned long index,
			unsigned long loopLength,
			unsigned int endRecordSamps);
		void EndRecording();
		void Ditch();
		void Overdub();
		void PunchIn();
		void PunchOut();

	protected:
		virtual std::vector<actions::JobAction> _CommitChanges() override;

		void Reset();
		unsigned long LoopIndex() const;
		static double CalcDrawRadius(unsigned long loopLength);
		void UpdateLoopModel();

	protected:
		static const unsigned int _InitBufferSize = 1000000;

		bool _modelNeedsUpdating;
		bool _endRecordingCompleted;
		unsigned long _playIndex;
		float _lastPeak;
		double _pitch;
		unsigned long _loopLength;
		unsigned int _endRecordSampCount;
		unsigned int _endRecordSamps;
		LoopVisualState _state;
		LoopParams _loopParams;
		std::shared_ptr<audio::AudioMixer> _mixer;
		std::shared_ptr<LoopModel> _model;
		std::shared_ptr<VU> _vu;
		std::vector<float> _buffer;
		std::vector<float> _backBuffer;
	};
}
