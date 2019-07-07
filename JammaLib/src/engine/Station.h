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
		virtual MultiAudioDirection MultiAudibleDirection() const override
		{
			return MULTIAUDIO_BOTH;
		}
		virtual void OnPlay(const std::shared_ptr<base::MultiAudioSink> dest, unsigned int numSamps) override;
		virtual void OnWrite(const std::shared_ptr<base::MultiAudioSource> src, unsigned int numSamps) override;
		virtual actions::ActionResult OnAction(actions::KeyAction action) override;
		virtual actions::ActionResult OnAction(actions::TouchAction action) override;
		virtual actions::ActionResult OnAction(actions::TriggerAction action) override;
		virtual void OnTick(Time curTime, unsigned int samps) override;
		
		void AddTake(LoopTakeParams takeParams);
		void AddTrigger(TriggerParams trigParams);
		void Reset();

	protected:
		std::shared_ptr<LoopTake> GetLoopTake(unsigned long id);
		std::shared_ptr<LoopTake> AddLoopTake(actions::TriggerAction action);

	protected:
		std::shared_ptr<Timer> _globalClock;
		std::vector<std::shared_ptr<LoopTake>> _loopTakes;
		std::vector<std::shared_ptr<Trigger>> _triggers;
	};
}
