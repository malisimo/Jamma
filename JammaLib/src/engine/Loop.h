#pragma once

#include <memory>
#include "Audible.h"
#include "ActionReceiver.h"
#include "../resources/WavResource.h"

class Loop :
	public Audible, public ActionReceiver
{
public:
	Loop();
	~Loop();

public:
	virtual void Play(float* buf, unsigned int numChans, unsigned int numSamps) override;

private:
	unsigned int _index;
	std::weak_ptr<WavResource> _wav;
};

