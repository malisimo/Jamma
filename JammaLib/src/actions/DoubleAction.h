#pragma once

#include "Action.h"
#include "../base/ActionUndo.h"

namespace actions
{
	class DoubleAction :
		public base::Action
	{
	public:
		DoubleAction(double value);
		~DoubleAction();

		double Value() const;

	public:
		double _value;
	};

	class DoubleActionUndo :
		public base::ActionUndo
	{
	public:
		DoubleActionUndo(double value,
			std::weak_ptr<base::ActionSender> sender);
		~DoubleActionUndo();

	public:
		virtual base::UndoType UndoType() const override
		{
			return base::UNDO_DOUBLE;
		}

		double Value() const;

	public:
		double _value;
	};

}
