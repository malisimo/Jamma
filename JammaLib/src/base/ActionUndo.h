#pragma once

#include <memory>
#include "ActionSender.h"

namespace base
{
	enum UndoType
	{
		UNDO_DEFAULT,
		UNDO_DOUBLE
	};

	class ActionUndo :
		public std::enable_shared_from_this<ActionUndo>
	{
	public:
		ActionUndo(std::weak_ptr<ActionSender> sender) :
			_sender(sender) {};
		~ActionUndo() {};

		// Copy
		//ActionUndo(const ActionUndo &) = delete;
		//ActionUndo& operator=(const ActionUndo &) = delete;
		ActionUndo(const ActionUndo& other)
		{
			_sender = other._sender;
		}

		ActionUndo& operator=(const ActionUndo& other)
		{
			if (&other == this)
				return *this;

			_sender = other._sender;
			return *this;
		}

		// Move
		ActionUndo(ActionUndo&& other) :
			_sender(other._sender)
		{
			other._sender = std::weak_ptr<ActionSender>();
		}

		ActionUndo& operator=(ActionUndo&& other)
		{
			if (this != &other)
			{
				_sender.swap(other._sender);
			}

			return *this;
		}

	public:
		virtual UndoType UndoType() const
		{
			return UNDO_DEFAULT;
		}

		virtual bool Undo()
		{
			auto snd = _sender.lock();
			if (snd)
				return snd->Undo(this->shared_from_this());

			return false;
		}

		virtual bool Redo()
		{
			auto snd = _sender.lock();
			if (snd)
				return snd->Redo(this->shared_from_this());

			return false;
		}

	private:
		std::weak_ptr<ActionSender> _sender;
	};
}
