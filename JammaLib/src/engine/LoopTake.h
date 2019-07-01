#pragma once

#include <vector>
#include <memory>
#include "Loop.h"
#include "GuiElement.h"
#include "MultiAudioSource.h"
#include "AudioSink.h"
#include "ActionUndo.h"

namespace engine
{
	class LoopTakeParams : public base::GuiElementParams
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
		std::vector<LoopParams> Loops;
	};

	class LoopTake :
		public base::GuiElement,
		public base::MultiAudioSource
	{
	public:
		LoopTake(LoopTakeParams params);
		~LoopTake();

		// Copy
		LoopTake(const LoopTake&) = delete;
		LoopTake& operator=(const LoopTake&) = delete;

	public:
		virtual void Play(const std::vector<std::shared_ptr<base::AudioSink>>& dest, unsigned int numSamps) override;
	/*	virtual void Draw(base::DrawContext& ctx) override;

	protected:
		virtual bool _InitResources(resources::ResourceLib& resourceLib) override;
		virtual bool _ReleaseResources() override;*/

		void AddLoop(LoopParams loopParams);

	protected:
		std::vector<std::shared_ptr<Loop>> _loops;
	};
}
