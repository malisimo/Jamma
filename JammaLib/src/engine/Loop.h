#pragma once

#include <string>
#include <memory>
#include "MultiAudioSource.h"
#include "ActionReceiver.h"
#include "ResourceUser.h"
#include "GuiElement.h"
#include "GlUtils.h"
#include "../utils/ArrayUtils.h"
#include "../gui/GuiModel.h"
#include "../io/FileReadWriter.h"
#include "../io/JamFile.h"
#include "../audio/AudioMixer.h"
#include "../graphics/GlDrawContext.h"
#include "../resources/WavResource.h"

namespace engine
{
	constexpr auto TWOPI = 6.283185307179586476925286766559;

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
			Wav(""),
			PlayTexture(""),
			RecordTexture(""),
			OverdubTexture(""),
			PunchTexture(""),
			Id(0),
			TakeId(0)
		{
		}

		LoopParams(base::GuiElementParams params,
			std::string wav) :
			base::GuiElementParams(params),
			Wav(wav),
			PlayTexture(""),
			RecordTexture(""),
			OverdubTexture(""),
			PunchTexture(""),
			Id(0),
			TakeId(0)
		{
		}

	public:
		std::string Wav;
		std::string PlayTexture;
		std::string RecordTexture;
		std::string OverdubTexture;
		std::string PunchTexture;
		unsigned long Id;
		unsigned long TakeId;
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
			_modelNeedsUpdating(other._modelNeedsUpdating),
			_playPos(other._playPos),
			_pitch(other._pitch),
			_length(other._length),
			_state(other._state),
			_loopParams{other._loopParams},
			_mixer(std::move(other._mixer)),
			_model(std::move(other._model)),
			_buffer(std::move(other._buffer)),
			_backBuffer(std::move(other._backBuffer))
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
				std::swap(_modelNeedsUpdating, other._modelNeedsUpdating);
				std::swap(_playPos, other._playPos);
				std::swap(_pitch, other._pitch);
				std::swap(_length, other._length);
				std::swap(_state, other._state);
				std::swap(_guiParams, other._guiParams);
				std::swap(_index, other._index);
				std::swap(_loopParams, other._loopParams);
				_mixer.swap(other._mixer);
				_model.swap(other._model);
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

		void OnPlayRaw(const std::shared_ptr<base::MultiAudioSink> dest,
			unsigned int channel,
			unsigned int delaySamps,
			unsigned int numSamps);
		unsigned int InputChannel() const;
		void SetInputChannel(unsigned int channel);
		unsigned long Id() const;

		bool Load(const io::WavReadWriter& readWriter);
		void Record();
		void Play(unsigned long index, unsigned long length);
		void Ditch();
		void Overdub();
		void PunchIn();
		void PunchOut();

	protected:
		virtual void _CommitChanges() override;

		void Reset();
		void UpdateLoopModel();
		double CalcDrawRadius();
		std::tuple<std::vector<float>, std::vector<float>, float, float>
			CalcGrainGeometry(unsigned int grain,
			unsigned int numGrains,
			float lastYMin,
			float lastYMax,
			float radius);

	protected:
		static const unsigned int _MaxFadeSamps = 30000;
		static const unsigned int _InitBufferSize = 1000000;
		static const unsigned int _MaxBufferSize = 40000000;
		static const unsigned int _GrainSamps = 1100;

		bool _modelNeedsUpdating;
		unsigned long _playPos;
		double _pitch;
		unsigned long _length;
		LoopVisualState _state;
		LoopParams _loopParams;
		std::shared_ptr<audio::AudioMixer> _mixer;
		std::shared_ptr<gui::GuiModel> _model;
		std::vector<float> _buffer;
		std::vector<float> _backBuffer;
	};
}
