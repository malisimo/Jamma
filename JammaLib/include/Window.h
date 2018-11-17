///////////////////////////////////////////////////////////
//
// Copyright(c) 2018-2019 Matt Jones
// Subject to the MIT license, see LICENSE file.
//
///////////////////////////////////////////////////////////

#pragma once

#include <windows.h>

class Window
{
public:
	struct Config
	{
		int Width;
		int	Height;
		int PosX;
		int PosY;
		bool Windowed;
	};

private:
	LPTSTR _windowClass;	// Window Class
	HGLRC _rc;			// Rendering Context
	HDC	_dc;				// Device Context
	HWND _wnd;			// Window
	DWORD _style;
	Config _config;

public:
	Window();
	~Window();

	void ShowMessage(LPCWSTR message);
	int Create(HINSTANCE hInstance, int nCmdShow);
	ATOM RegisterClass(HINSTANCE hInstance);
	void AdjustSize();
	void Center();
	void Render();
	void Swap();
	void Destroy();
	Config GetConfig();

	static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};