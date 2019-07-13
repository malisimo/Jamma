#pragma once

#include "Sharable.h"

namespace base
{
	class Actionable :
		public virtual Sharable
	{
	public:
		enum ActionDirection
		{
			ACTIONDIR_NONE,
			ACTIONDIR_RECEIVE,
			ACTIONDIR_SEND,
			ACTIONDIR_DUPLEX
		};

	public:
		Actionable() {};
		~Actionable() {};

	public:
		virtual ActionDirection Direction() const {	return ACTIONDIR_NONE;	}
	};
}
