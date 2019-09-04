#pragma once

#include <string>
#include "Sharable.h"
#include "../resources/Resource.h"
#include "../resources/ResourceLib.h"

namespace base
{
	class ResourceUser;

	class ResourceUserParams
	{
	public:
		std::function<void(std::shared_ptr<ResourceUser>)> UpdateResourceFunc;
	};

	class ResourceUser :
		public virtual Sharable
	{
	public:
		ResourceUser(ResourceUserParams resourceParams) :
			_resourcesInitialised(false),
			_resourceParams(resourceParams)
		{ }

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
		
		std::shared_ptr<ResourceUser> shared_from_this()
		{
			return std::dynamic_pointer_cast<ResourceUser>(
				Sharable::shared_from_this());
		}

	protected:
		virtual bool _InitResources(resources::ResourceLib& resourceLib) { return true; };
		virtual bool _ReleaseResources() { return true; };

		bool _resourcesInitialised;
		ResourceUserParams _resourceParams;
	};
}
