///////////////////////////////////////////////////////////
//
// Copyright(c) 2018-2019 Matt Jones
// Subject to the MIT license, see LICENSE file.
//
///////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <windows.h>
#include <shlobj.h>

namespace utils
{
	enum PathType
	{
		PATH_ROAMING
	};

	std::wstring GetPath(PathType pathType);
}
