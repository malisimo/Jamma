#include "LoopTake.h"

using namespace engine;
using base::AudioSink;
using base::DrawContext;
using base::Drawable;
using base::DrawableParams;
using resources::ResourceLib;

LoopTake::LoopTake(DrawableParams params) :
	Drawable(params),
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

void LoopTake::Draw(DrawContext& ctx)
{
}

bool LoopTake::InitResources(ResourceLib & resourceLib)
{
	for (auto& loop : _loops)
		loop->InitResources(resourceLib);

	return true;
}

bool LoopTake::ReleaseResources()
{
	for (auto& loop : _loops)
		loop->ReleaseResources();

	return true;
}

void LoopTake::AddLoop(std::unique_ptr<Loop> loop)
{
	_loops.push_back(std::move(loop));
}
