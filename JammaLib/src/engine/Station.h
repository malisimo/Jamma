#pragma once

#include "MultiAudioSource.h"
#include "LoopTake.h"
#include "Trigger.h"
#include "AudioSink.h"

namespace engine
{
	class Station :
		public base::Drawable,
		public base::MultiAudioSource
	{
	public:
		Station(base::DrawableParams params);
		~Station();

	public:
		virtual void Play(const std::vector<std::shared_ptr<base::AudioSink>>& dest, unsigned int numSamps) override;
		virtual void Draw(base::DrawContext& ctx) override;
		virtual bool InitResources(resources::ResourceLib& resourceLib) override;
		virtual bool ReleaseResources() override;

		void AddTake(std::unique_ptr<LoopTake> take);

	protected:
		std::vector<std::unique_ptr<LoopTake>> _loopTakes;
		std::vector<Trigger> _triggers;
	};
}
