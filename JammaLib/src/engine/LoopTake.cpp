#include "LoopTake.h"

using namespace engine;
using base::AudioSink;
using base::DrawContext;
using base::Drawable;
using base::DrawableParams;
using resources::ResourceLib;

LoopTake::LoopTake(LoopTakeParams params) :
	GuiElement(params),
	MultiAudioSource(),
	_loops()
{
}

LoopTake::~LoopTake()
{
}

void LoopTake::Play(const std::vector<std::shared_ptr<AudioSink>>& dest, unsigned int numSamps)
{
	for (auto& loop : _loops)
		loop->Play(dest, numSamps);
}

void LoopTake::AddLoop(LoopParams loopParams)
{
	auto loop = std::make_shared<Loop>(loopParams);
	_loops.push_back(loop);
	_children.push_back(loop);
}
