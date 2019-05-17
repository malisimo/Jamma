#pragma once

#include <string>
#include "../resources/Resource.h"
#include "../resources/ResourceLib.h"

namespace base
{
	class ResourceUser
	{
	public:
		bool InitResources(resources::ResourceLib& resourceLib)
		{
			_resourcesInitialised = _InitResources(resourceLib);
			return _resourcesInitialised;
		};

		bool ReleaseResources()
		{
			auto res = _resourcesInitialised ? _ReleaseResources() : false;
			_resourcesInitialised = false;

			return res;
		};

	protected:
		virtual bool _InitResources(resources::ResourceLib& resourceLib) { return true; };
		virtual bool _ReleaseResources() { return true; };

		bool _resourcesInitialised;
	};
}
