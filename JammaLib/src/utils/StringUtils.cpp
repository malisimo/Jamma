///////////////////////////////////////////////////////////
//
// Copyright(c) 2018-2019 Matt Jones
// Subject to the MIT license, see LICENSE file.
//
///////////////////////////////////////////////////////////

#include <windows.h>
#include "StringUtils.h"

LPCWSTR CharsToUnicodeString(const char* str)
{
	size_t size = strlen(str) + 1;
	wchar_t* wideStr = new wchar_t[size];

	size_t outSize;
	mbstowcs_s(&outSize, wideStr, size, str, size - 1);

	return wideStr;
}