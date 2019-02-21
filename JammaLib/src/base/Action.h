#pragma once

#include <memory>

class ActionSender;
class ActionReceiver;

class Action
{
public:
	Action();
	~Action();

	// Copy
	//Action(const Action &) = delete;
	//Action& operator=(const Action &) = delete;
	Action(const Action &other)
	{
		_sender = other._sender;
		_receiver = other._receiver;
	}
	Action& operator=(const Action &other)
	{
		if (&other == this)
			return *this;

		_sender = other._sender;
		_receiver = other._receiver;
		return *this;
	}

	// Move
	Action(Action &&other) :
		_sender(other._sender),
		_receiver(other._receiver)
	{
		other._sender = std::weak_ptr<ActionSender>();
		other._receiver = std::weak_ptr<ActionReceiver>();
	}

	Action& operator=(Action &&other)
	{
		if (this != &other)
		{
			_sender.swap(other._sender);
			_receiver.swap(other._receiver);
		}

		return *this;
	}

	virtual auto GetReceiver() -> std::weak_ptr<ActionReceiver>
	{
		return _receiver;
	}

protected:
	std::weak_ptr<ActionSender> _sender;
	std::weak_ptr<ActionReceiver> _receiver;
};

