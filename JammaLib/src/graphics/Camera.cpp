#include "Camera.h"

using namespace base;
using namespace graphics;

Camera::Camera(CameraParams params) :
	Moveable(params),
	_id(params.Id)
{
}
