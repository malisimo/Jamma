#pragma once
#include "CommonTypes.h"

namespace base
{
	class MoveableParams
	{
	public:
		utils::Position2d Position;
	};

	class Moveable
	{
	public:
		Moveable(MoveableParams params);
		~Moveable();

	public:
		virtual utils::Position2d Position() const;
		virtual void SetPosition(utils::Position2d pos);

	protected:
		MoveableParams _moveParams;
	};
}
