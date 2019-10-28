///////////////////////////////////////////////////////////
//
// Copyright(c) 2018-2019 Matt Jones
// Subject to the MIT license, see LICENSE file.
//
///////////////////////////////////////////////////////////

#pragma once

namespace utils
{
	unsigned int ModNeg(int v, unsigned int len)
	{
		return (v % len + len) % len;
	}

	unsigned long ModNeg(int v, unsigned long len)
	{
		return (v % len + len) % len;
	}
}
