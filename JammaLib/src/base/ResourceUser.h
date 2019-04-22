#pragma once

#include <string>
#include "../resources/Resource.h"
#include "../resources/ResourceLib.h"

namespace base
{
	class ResourceUser
	{
	public:
		ResourceUser();
		~ResourceUser();

	public:
		virtual bool InitResources(resources::ResourceLib& resourceLib);
		virtual bool ReleaseResources();
	};
}
