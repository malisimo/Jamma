#pragma once
#include "CommonTypes.h"

class SizeableParams
{
public:
	Size2d Size;
	Size2d MinSize;
};

class Sizeable
{
public:
	Sizeable(SizeableParams params);
	~Sizeable();

public:
	virtual void SetSize(Size2d size);
	virtual Size2d GetSize() const;
	virtual void SetMinSize(Size2d size);
	virtual Size2d GetMinSize() const;
	virtual bool IsResizing() const;
	virtual void SetResizing(bool resizing);

protected:
	SizeableParams _sizeParams;
	bool _resizing;
};
