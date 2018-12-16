///////////////////////////////////////////////////////////
//
// Copyright(c) 2018-2019 Matt Jones
// Subject to the MIT license, see LICENSE file.
//
///////////////////////////////////////////////////////////

#pragma once

#include <windows.h>
#include <memory>
#include "glew/glew.h"
#include "gl/gl.h"
#include "gl/glext.h"
#include "gl/wglext.h"
#include "Scene.h"

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

public:
	Window(Scene& scene);
	~Window();

private:
	LPTSTR _windowClass;	// Window Class
	HGLRC _rc;			// Rendering Context
	HDC	_dc;				// Device Context
	HWND _wnd;			// Window
	DWORD _style;
	Config _config;

	GlDrawContext _drawContext;
	Scene& _scene;

public:
	void InitScene();
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

private:
	static void APIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void * userParam);
};