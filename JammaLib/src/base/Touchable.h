#pragma once

namespace base
{
	class TouchableParams
	{
	};

	class Touchable
	{
	public:
		Touchable(TouchableParams params);
		~Touchable();

		enum TouchType
		{
			MOUSE,
			FINGER
		};

	public:
		virtual void OnTouchBegin(TouchType touchType, int num);
		virtual void OnTouchEnd(TouchType touchType, int num);
		virtual void OnDrag(TouchType touchType, int num);
	};
}
