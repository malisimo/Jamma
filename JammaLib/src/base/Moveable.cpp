#include "Moveable.h"

Moveable::Moveable(MoveableParams params) :
	_moveParams(params)
{
}

Moveable::~Moveable()
{
}

Position2d Moveable::Position() const
{
	return _moveParams.Position;
}

void Moveable::SetPosition(Position2d pos)
{
	_moveParams.Position = pos;
}
