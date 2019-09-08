#pragma once

#include <string>
#include "Sharable.h"
#include "../resources/Resource.h"
#include "../resources/ResourceLib.h"

namespace base
{
	class ResourceUser
	{
	public:
		ResourceUser() :
			_resourcesInitialised(false)
		{ }

	public:
		virtual void InitResources(resources::ResourceLib& resourceLib, bool forceInit)
		{
			if (forceInit)
			{
				ReleaseResources();

				_InitResources(resourceLib, true);
				_resourcesInitialised = true;
			}
			else
			{
				if (!_resourcesInitialised)
					_InitResources(resourceLib, false);

				_resourcesInitialised = true;
			}
		};

		void ReleaseResources()
		{
			if (_resourcesInitialised)
				_ReleaseResources();

			_resourcesInitialised = false;
		};

	protected:
		virtual void _InitResources(resources::ResourceLib& resourceLib, bool forceInit) { };
		virtual void _ReleaseResources() { };

	private:
		bool _resourcesInitialised;
	};
}
