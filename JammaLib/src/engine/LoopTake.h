#pragma once

#include <vector>
#include <memory>
#include "Loop.h"
#include "GuiElement.h"
#include "MultiAudioSource.h"
#include "MultiAudioSink.h"
#include "AudioSink.h"
#include "ActionUndo.h"
#include "Trigger.h"

namespace engine
{
	class LoopTakeParams :
		public base::GuiElementParams
	{
	public:
		LoopTakeParams() :
			base::GuiElementParams(DrawableParams{ "" },
				MoveableParams(utils::Position2d{ 0, 0 }, utils::Position3d{ 0, 0, 0 }, 1.0),
				SizeableParams{ 1,1 },
				"",
				"",
				"",
				{}),
			Loops({})
		{
		}

		LoopTakeParams(base::GuiElementParams params,
			std::vector<LoopParams> loops) :
			base::GuiElementParams(params),
			Loops(loops)
		{
		}

	public:
		unsigned long Id;
		std::vector<LoopParams> Loops;
	};

	class LoopTake :
		public virtual base::GuiElement,
		public virtual base::MultiAudioSource,
		public virtual base::MultiAudioSink
	{
	public:
		enum LoopTakeSource
		{
			SOURCE_ADC,
			SOURCE_STATION,
			SOURCE_LOOPTAKE
		};

		enum LoopTakeState
		{
			STATE_DEFAULT,
			STATE_RECORDING
		};

	public:
		LoopTake(LoopTakeParams params);
		~LoopTake();

		// Copy
		LoopTake(const LoopTake&) = delete;
		LoopTake& operator=(const LoopTake&) = delete;

	public:
		static std::optional<std::shared_ptr<LoopTake>> FromFile(LoopTakeParams takeParams, io::JamFile::LoopTake takeStruct, std::wstring dir);

		virtual utils::Position2d Position() const override;
		virtual MultiAudioDirection MultiAudibleDirection() const override { return MULTIAUDIO_BOTH; }
		virtual void OnPlay(const std::shared_ptr<base::MultiAudioSink> dest, unsigned int numSamps) override;
		virtual void EndMultiPlay(unsigned int numSamps) override;
		// TODO: Remove OnWrite method
		virtual void OnWrite(const std::shared_ptr<base::MultiAudioSource> src, unsigned int numSamps) override;
		virtual void OnWriteChannel(unsigned int channel,
			const std::shared_ptr<base::AudioSource> src,
			unsigned int numSamps);
		virtual void EndMultiWrite(unsigned int numSamps, bool updateIndex) override;

		void OnPlayRaw(const std::shared_ptr<MultiAudioSink> dest, unsigned int delaySamps, unsigned int numSamps);
		
		unsigned long Id() const;
		unsigned long SourceId() const;
		LoopTakeSource SourceType() const;
		unsigned long NumRecordedSamps() const;
		std::shared_ptr<Loop> AddLoop(unsigned int chan);
		void AddLoop(std::shared_ptr<Loop> loop);

		void Record(std::vector<unsigned int> channels);
		void Play(unsigned long index, unsigned long length);
		void Ditch();

	protected:
		static unsigned int CalcLoopHeight(unsigned int takeHeight, unsigned int numLoops);

		virtual bool _InitResources(resources::ResourceLib& resourceLib) override;
		void ArrangeLoops();

	protected:
		static const utils::Size2d _Gap;

		unsigned long _id;
		unsigned int _sourceId;
		LoopTakeSource _sourceType;
		unsigned long _recordedSampCount;
		std::vector<std::shared_ptr<Loop>> _loops;
	};
}
