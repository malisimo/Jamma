#pragma once

#include "Action.h"
#include "CommonTypes.h"

namespace actions
{
	class TriggerAction :
		public base::Action
	{
	public:
		TriggerAction();
		~TriggerAction();

	public:
		enum TriggerActionType
		{
			TRIGGER_REC_START,
			TRIGGER_REC_END,
			TRIGGER_DITCH,
			TRIGGER_ARM_START,
			TRIGGER_ARM_END,
			TRIGGER_OVERDUB_START,
			TRIGGER_OVERDUB_END
		};

	public:
		TriggerActionType ActionType;
	};
}
