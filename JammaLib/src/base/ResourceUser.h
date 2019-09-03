#pragma once

#include <string>
#include "Sharable.h"
#include "../resources/Resource.h"
#include "../resources/ResourceLib.h"

namespace base
{
	class ResourceUser :
		public virtual Sharable
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
		void SetUpdateResourceFunc(std::function<void(std::shared_ptr<ResourceUser>)> func) { _updateResourceFunc = func; }

	protected:
		virtual bool _InitResources(resources::ResourceLib& resourceLib) { return true; };
		virtual bool _ReleaseResources() { return true; };

		bool _resourcesInitialised;
		std::function<void(std::shared_ptr<ResourceUser>)> _updateResourceFunc;
	};
}
