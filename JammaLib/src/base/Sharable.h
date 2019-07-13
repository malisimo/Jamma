#pragma once

#include <memory>
#include <string>

namespace base
{
	class Sharable :
		public std::enable_shared_from_this<Sharable>
	{
	public:
		Sharable() {}
		~Sharable() {}

	public:
		virtual std::string ClassName() const {	return "Sharable"; }
	};
}