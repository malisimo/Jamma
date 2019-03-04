#pragma once

#include <string>
#include "../resources/Resource.h"
#include "../resources/ResourceLib.h"

class ResourceUser
{
public:
	ResourceUser();
	~ResourceUser();

public:
	virtual bool InitResources(ResourceLib& resourceLib);
	virtual bool ReleaseResources();
};

