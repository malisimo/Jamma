#include "ResourceUser.h"

using namespace base;
using resources::ResourceLib;

ResourceUser::ResourceUser()
{
}

ResourceUser::~ResourceUser()
{
}

bool ResourceUser::InitResources(ResourceLib& resourceLib)
{
	return true;
}

bool ResourceUser::ReleaseResources()
{
	return true;
}
