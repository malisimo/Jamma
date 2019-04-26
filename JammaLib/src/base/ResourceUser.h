#pragma once

#include <string>
#include "../resources/Resource.h"
#include "../resources/ResourceLib.h"

namespace base
{
	class ResourceUser
	{
	public:
		virtual bool InitResources(resources::ResourceLib& resourceLib) { return true; };
		virtual bool ReleaseResources() { return true; };
	};
}
