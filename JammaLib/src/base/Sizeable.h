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
		Sizeable(SizeableParams params) :
			_sizeParams(params),
			_resizing(false)
		{
		};

	public:
		virtual void SetSize(utils::Size2d size)
		{
			_sizeParams.Size = size;
		};
		virtual utils::Size2d GetSize() const
		{
			return _sizeParams.Size;
		}
		virtual void SetMinSize(utils::Size2d size)
		{
			_sizeParams.MinSize = size;

			if (_sizeParams.MinSize.Width < size.Width)
				_sizeParams.Size.Width = _sizeParams.MinSize.Width;

			if (_sizeParams.MinSize.Height < size.Height)
				_sizeParams.Size.Height = _sizeParams.MinSize.Height;
		}
		virtual utils::Size2d GetMinSize() const
		{
			return _sizeParams.MinSize;
		}
		virtual bool IsResizing() const
		{
			return _resizing;
		}
		virtual void SetResizing(bool resizing)
		{
			_resizing = resizing;
		}
	
	protected:
		SizeableParams _sizeParams;
		bool _resizing;
	};
}
