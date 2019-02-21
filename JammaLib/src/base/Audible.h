#pragma once

class AudibleParams
{
};

class Audible
{
public:
	Audible(AudibleParams params);
	~Audible();

protected:
	AudibleParams _audibleParams;
};

