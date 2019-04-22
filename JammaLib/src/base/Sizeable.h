#pragma once

#include "CommonTypes.h"

namespace base
{
	class SizeableParams
	{
	public:
		utils::Size2d Size;
		utils::Size2d MinSize;
	};

	class Sizeable
	{
	public:
		Sizeable(SizeableParams params);
		~Sizeable();

	public:
		virtual void SetSize(utils::Size2d size);
		virtual utils::Size2d GetSize() const;
		virtual void SetMinSize(utils::Size2d size);
		virtual utils::Size2d GetMinSize() const;
		virtual bool IsResizing() const;
		virtual void SetResizing(bool resizing);

	protected:
		SizeableParams _sizeParams;
		bool _resizing;
	};
}
