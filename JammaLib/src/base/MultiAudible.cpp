#include "MultiAudible.h"

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
