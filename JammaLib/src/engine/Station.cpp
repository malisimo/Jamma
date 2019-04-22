#include "Station.h"

using namespace engine;
using base::MultiAudible;
using base::MultiAudibleParams;

Station::Station() :
	MultiAudible(MultiAudibleParams{}),
	_loopTakes({})
{
}

Station::~Station()
{
}
