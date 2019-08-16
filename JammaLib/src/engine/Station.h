#pragma once

#include "LoopTake.h"
#include "Trigger.h"
#include "AudioSink.h"
#include "MultiAudioSource.h"
#include "MultiAudioSink.h"

namespace engine
{
	class StationParams :
		public base::GuiElementParams
	{
	public:
		StationParams() :
			base::GuiElementParams(DrawableParams{ "" },
				MoveableParams{ 0,0 },
				SizeableParams{ 1,1 },
				"",
				"",
				"",
				{}),
			GlobalClock(std::shared_ptr<Timer>())
		{
		}

	public:
		std::shared_ptr<Timer> GlobalClock;
	};
	
	class Station :
		public base::Tickable,
		public base::GuiElement,
		public base::MultiAudioSource,
		public base::MultiAudioSink
	{
	public:
		Station(StationParams params);
		~Station();

		// Copy
		Station(const Station&) = delete;
		Station& operator=(const Station&) = delete;

	public:
		static std::optional<std::shared_ptr<Station>> FromFile(StationParams stationParams, io::JamFile::Station stationStruct);

		virtual MultiAudioDirection MultiAudibleDirection() const override { return MULTIAUDIO_BOTH; }
		virtual void OnPlay(const std::shared_ptr<base::MultiAudioSink> dest, unsigned int numSamps) override;
		virtual void EndMultiPlay(unsigned int numSamps) override;
		virtual void OnWrite(const std::shared_ptr<base::MultiAudioSource> src, unsigned int numSamps) override;
		virtual void EndMultiWrite(unsigned int numSamps, bool updateIndex) override;
		virtual actions::ActionResult OnAction(actions::KeyAction action) override;
		virtual actions::ActionResult OnAction(actions::TouchAction action) override;
		virtual actions::ActionResult OnAction(actions::TriggerAction action) override;
		virtual void OnTick(Time curTime, unsigned int samps) override;
		
		std::shared_ptr<LoopTake> AddTake();
		void AddTake(std::shared_ptr<LoopTake> take);
		std::shared_ptr<Trigger> AddTrigger(TriggerParams trigParams);
		void Reset();

	protected:
		static unsigned int CalcTakeHeight(unsigned int stationHeight, unsigned int numTakes);
		
		std::optional<std::shared_ptr<LoopTake>> TryGetTake(unsigned long id);

	protected:
		static const utils::Size2d _Gap;

		std::shared_ptr<Timer> _globalClock;
		std::vector<std::shared_ptr<LoopTake>> _loopTakes;
		std::vector<std::shared_ptr<Trigger>> _triggers;
	};
}
