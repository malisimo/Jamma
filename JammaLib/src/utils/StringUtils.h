///////////////////////////////////////////////////////////
//
// Copyright(c) 2018-2019 Matt Jones
// Subject to the MIT license, see LICENSE file.
//
///////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <windows.h>

namespace utils
{
	LPCWSTR CharsToUnicodeString(const char* str);
	std::wstring CharsToWideString(const char* str);

	std::string EncodeUtf8(const std::wstring& wStr);
	std::wstring DecodeUtf8(const std::string& str);
}
