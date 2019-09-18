#include "JobAction.h"

using namespace actions;

JobAction::JobAction() :
	JobActionType(JOB_RENDERWAVE),
	Receiver(std::shared_ptr<base::ActionReceiver>())
{
}

JobAction::~JobAction()
{
}
