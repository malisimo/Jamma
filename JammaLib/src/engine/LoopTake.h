#pragma once

#include <vector>
#include <memory>
#include "Loop.h"
#include "Drawable.h"
#include "MultiAudioSource.h"
#include "AudioSink.h"

namespace engine
{
	class LoopTake :
		public base::Drawable,
		public base::MultiAudioSource
	{
	public:
		LoopTake(base::DrawableParams params);
		~LoopTake();

		// Copy
		LoopTake(const LoopTake&) = delete;
		LoopTake& operator=(const LoopTake&) = delete;

	public:
		virtual void Play(const std::vector<std::shared_ptr<base::AudioSink>>& dest, unsigned int numSamps) override;
		virtual void Draw(base::DrawContext& ctx) override;
		virtual bool InitResources(resources::ResourceLib& resourceLib) override;
		virtual bool ReleaseResources() override;

		void AddLoop(std::unique_ptr<Loop> loop);

	protected:
		std::vector<std::unique_ptr<Loop>> _loops;
	};
}
