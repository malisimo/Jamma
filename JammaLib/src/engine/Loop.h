#pragma once
#include "Audible.h"
#include "ActionReceiver.h"

class Loop :
	public Audible, public ActionReceiver
{
public:
	Loop();
	~Loop();
};

