#include "MultiAudible.h"

using namespace base;
using namespace audio;

MultiAudible::MultiAudible(MultiAudibleParams params) :
	_multiAudibleParams(params),
	_outputBuffers({})
{
}

MultiAudible::~MultiAudible()
{
}

void MultiAudible::Play(std::vector<AudioBuffer>& buf, unsigned int numSamps)
{
}
