#pragma once

#include <memory>

namespace base
{
	class ActionReceiver;

	class Action
	{
	public:
		Action() {};
		~Action() {};

		// Copy
		//Action(const Action &) = delete;
		//Action& operator=(const Action &) = delete;
		Action(const Action& other)
		{
			_receiver = other._receiver;
		}
		Action& operator=(const Action& other)
		{
			if (&other == this)
				return *this;

			_receiver = other._receiver;
			return *this;
		}

		// Move
		Action(Action && other) :
			_receiver(other._receiver)
		{
			other._receiver = std::weak_ptr<ActionReceiver>();
		}

		Action& operator=(Action && other)
		{
			if (this != &other)
			{
				_receiver.swap(other._receiver);
			}

			return *this;
		}

	protected:
		std::weak_ptr<ActionReceiver> _receiver; // Not used???
	};
}
