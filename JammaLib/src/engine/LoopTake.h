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
				MoveableParams{ 0,0 },
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
		virtual MultiAudioDirection MultiAudibleDirection() const override { return MULTIAUDIO_BOTH; }
		virtual void OnPlay(const std::shared_ptr<base::MultiAudioSink> dest, unsigned int numSamps) override;
		virtual void EndMultiPlay(unsigned int numSamps) override;
		virtual void OnWrite(const std::shared_ptr<base::MultiAudioSource> src, unsigned int numSamps) override;
		virtual void EndMultiWrite(unsigned int numSamps, bool updateIndex) override;

		void OnPlayRaw(const std::shared_ptr<MultiAudioSink> dest, unsigned int delaySamps, unsigned int numSamps);
		
		unsigned long Id() const;
		unsigned long SourceId() const;
		LoopTakeSource SourceType() const;
		unsigned long NumRecordedSamps() const;
		void AddLoop(LoopParams loopParams);

		void Record(std::vector<unsigned int> channels);
		void Play(unsigned long index, unsigned long length);
		void Ditch();

	protected:
		unsigned long _id;
		unsigned int _sourceId;
		LoopTakeSource _sourceType;
		unsigned long _recordedSampCount;
		std::vector<std::shared_ptr<Loop>> _loops;
	};
}
