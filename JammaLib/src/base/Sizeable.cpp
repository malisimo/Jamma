#include "Sizeable.h"

Sizeable::Sizeable(SizeableParams params) :
	_sizeParams(params),
	_resizing(false)
{
}

Sizeable::~Sizeable()
{
}

void Sizeable::SetSize(Size2d size)
{
	_sizeParams.Size = size;
}

Size2d Sizeable::GetSize() const
{
	return _sizeParams.Size;
}

void Sizeable::SetMinSize(Size2d size)
{
	_sizeParams.MinSize = size;

	if (_sizeParams.MinSize.Width < size.Width)
		_sizeParams.Size.Width = _sizeParams.MinSize.Width;

	if (_sizeParams.MinSize.Height < size.Height)
		_sizeParams.Size.Height = _sizeParams.MinSize.Height;
}

Size2d Sizeable::GetMinSize() const
{
	return _sizeParams.MinSize;
}

bool Sizeable::IsResizing() const
{
	return _resizing;
}

void Sizeable::SetResizing(bool resizing)
{
	_resizing = resizing;
}