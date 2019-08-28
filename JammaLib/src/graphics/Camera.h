#pragma once

#include "Moveable.h"

namespace graphics
{
	class CameraParams : public base::MoveableParams
	{
	public:
		CameraParams(base::MoveableParams moveParams,
			unsigned int id) :
			base::MoveableParams(moveParams),
			Id(id)
		{}

	public:
		unsigned int Id;
	};

	class Camera :
		public base::Moveable
	{
	public:
		Camera(CameraParams camParams);

	private:
		unsigned int _id;
	};
}
