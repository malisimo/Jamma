#pragma once
#include "CommonTypes.h"

class MoveableParams
{
public:
	Position2d Position;
};

class Moveable
{
public:
	Moveable(MoveableParams params);
	~Moveable();

public:
	virtual Position2d Position() const;
	virtual void SetPosition(Position2d pos);

protected:
	MoveableParams _moveParams;
};

