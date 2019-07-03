#pragma once

#include "MultiAudioSource.h"
#include "LoopTake.h"
#include "Trigger.h"
#include "AudioSink.h"

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
				{})
		{
		}
	};
		
	class Station :
		public base::Tickable,
		public base::GuiElement,
		public base::MultiAudioSource
	{
	public:
		Station(StationParams params);
		~Station();

		// Copy
		Station(const Station&) = delete;
		Station& operator=(const Station&) = delete;

	public:
		virtual void Play(const std::vector<std::shared_ptr<base::AudioSink>>& dest, unsigned int numSamps) override;
		virtual actions::ActionResult OnAction(actions::KeyAction action) override;
		virtual actions::ActionResult OnAction(actions::TouchAction action) override;
		virtual void OnTick(Time curTime, unsigned int samps) override;
		
		void AddTake(LoopTakeParams takeParams);
		void AddTrigger(TriggerParams trigParams);

	protected:
		std::vector<std::shared_ptr<LoopTake>> _loopTakes;
		std::vector<std::shared_ptr<Trigger>> _triggers;
	};
}
