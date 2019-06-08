#pragma once

namespace base
{
	class Actionable :
		public std::enable_shared_from_this<Actionable>
	{
		enum Direction
		{
			ACTION_SENDER,
			ACTION_RECEIVER
		};
	public:
		Actionable() {};
		~Actionable() {};

	public:
		virtual Direction GetDirection()
		{
			return ACTION_SENDER;
		}
	};
}
