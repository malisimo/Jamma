#include "Station.h"

using namespace engine;
using base::MultiAudioSource;
using base::AudioSink;
using base::Drawable;
using base::DrawableParams;
using base::DrawContext;
using resources::ResourceLib;

Station::Station(DrawableParams params) :
	Drawable(params),
	MultiAudioSource(),
	_loopTakes(),
	_triggers({})
{
}

Station::~Station()
{
}

void Station::Play(const std::vector<std::shared_ptr<base::AudioSink>>& dest, unsigned int numSamps)
{
	for (auto& take : _loopTakes)
		take->Play(dest, numSamps);
}

void Station::Draw(DrawContext& ctx)
{
	for (auto& take : _loopTakes)
		take->Draw(ctx);
}

bool Station::InitResources(ResourceLib & resourceLib)
{
	for (auto& take : _loopTakes)
		take->InitResources(resourceLib);

	return true;
}

bool Station::ReleaseResources()
{
	for (auto& take : _loopTakes)
		take->ReleaseResources();

	return true;
}

void Station::AddTake(std::unique_ptr<LoopTake> take)
{
	_loopTakes.push_back(std::move(take));
}