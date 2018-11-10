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
	LPTSTR windowClass;	// Window Class
	HGLRC RC;			// Rendering Context
	HDC	DC;				// Device Context
	HWND WND;			// Window
	DWORD style;

	struct Config
	{
		int width;
		int	height;
		int posX;
		int posY;
		bool windowed;
	} config;

	Window();
	~Window();

	void showMessage(LPCWSTR message);
	int create(HINSTANCE hInstance, int nCmdShow);
	ATOM registerClass(HINSTANCE hInstance);
	void adjustSize();
	void center();
	void render();
	void swapBuffers();
	void destroy();

	static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};