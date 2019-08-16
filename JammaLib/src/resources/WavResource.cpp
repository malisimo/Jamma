#include "WavResource.h"

using namespace resources;
using io::WavReadWriter;

WavResource::WavResource(std::string name, std::vector<float> audioBuf, unsigned int numSamps, unsigned int sampleRate) :
	Resource(name),
	_numSamps(numSamps),
	_sampleRate(sampleRate),
	_audioBuf(std::move(audioBuf))
{
}

WavResource::~WavResource()
{
}

unsigned int WavResource::Length() const
{
	return _numSamps;
}

const std::vector<float>& WavResource::Buffer()
{
	return _audioBuf;
}

std::optional<std::tuple<std::vector<float>, unsigned int, unsigned int>> WavResource::Load(const std::string & wavFile)
{
	return WavReadWriter().Read(wavFile, 80000000u);
}
