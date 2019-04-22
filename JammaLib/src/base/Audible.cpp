#include "Audible.h"

using namespace base;
using audio::AudioBuffer;

Audible::Audible(AudibleParams params) :
	_audibleParams(params)
{
}

Audible::~Audible()
{
}

void Audible::Play(std::shared_ptr<AudioBuffer> buf, unsigned int numSamps)
{
}
