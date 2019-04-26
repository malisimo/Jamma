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
		Moveable(MoveableParams params) : _moveParams(params) {};

	public:
		virtual void SetPosition(utils::Position2d pos)
		{
			_moveParams.Position = pos;
		};

		virtual utils::Position2d Position() const
		{
			return _moveParams.Position;
		};

	protected:
		MoveableParams _moveParams;
	};
}
