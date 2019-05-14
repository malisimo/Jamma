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
		//virtual void Draw(base::DrawContext& ctx) override;
		//virtual bool InitResources(resources::ResourceLib& resourceLib) override;
		//virtual bool ReleaseResources() override;

		void AddTake(LoopTakeParams takeParams);

	protected:
		std::vector<std::shared_ptr<LoopTake>> _loopTakes;
		std::vector<Trigger> _triggers;
	};
}
