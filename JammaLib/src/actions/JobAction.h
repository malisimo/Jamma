#pragma once

#include "Action.h"
#include "CommonTypes.h"

namespace base { class ActionReceiver; }

namespace actions
{
	class JobAction :
		public base::Action
	{
	public:
		JobAction();
		~JobAction();

		bool operator==(const JobAction& other) {
			if (other.JobActionType == JobActionType)
				return other.SourceId == SourceId;

			return false;
		}

	public:
		enum JobType
		{
			JOB_RENDERWAVE
		};

		JobType JobActionType;
		unsigned long SourceId;
		std::weak_ptr<base::ActionReceiver> Receiver;
	};
}
