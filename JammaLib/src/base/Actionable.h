#pragma once

namespace base
{
	class Actionable :
		public std::enable_shared_from_this<Actionable>
	{
	public:
		enum ActionDirection
		{
			ACTION_NONE,
			ACTION_RECEIVER,
			ACTION_SENDER,
			ACTION_BOTH
		};
	public:
		Actionable() {};
		~Actionable() {};

	public:
		virtual ActionDirection Direction() const
		{
			return ACTION_NONE;
		}
	};
}
