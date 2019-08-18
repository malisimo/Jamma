#pragma once
#include "CommonTypes.h"

namespace base
{
	class MoveableParams
	{
	public:
		MoveableParams():
			Position({0, 0}),
			ModelPosition({0, 0, 0}),
			ModelScale(1.0)
		{}

		MoveableParams(utils::Position2d pos,
			utils::Position3d modelPos,
			double modelScale) :
			Position(pos),
			ModelPosition(modelPos),
			ModelScale(modelScale)
		{}

	public:
		utils::Position2d Position;
		utils::Position3d ModelPosition;
		double ModelScale;
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

		virtual void SetModelPosition(utils::Position3d pos)
		{
			_moveParams.ModelPosition = pos;
		};

		virtual utils::Position3d ModelPosition() const
		{
			return _moveParams.ModelPosition;
		};

		virtual void SetModelScale(double scale)
		{
			_moveParams.ModelScale = scale;
		};

		virtual double ModelScale() const
		{
			return _moveParams.ModelScale;
		};

	protected:
		MoveableParams _moveParams;
	};
}
