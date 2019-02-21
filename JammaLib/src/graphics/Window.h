///////////////////////////////////////////////////////////
//
// Author 2019 Matt Jones
// Subject to the MIT license, see LICENSE file.
//
///////////////////////////////////////////////////////////

#pragma once

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include "glew/glew.h"
#include "gl/gl.h"
#include "gl/glext.h"
#include "gl/wglext.h"
#include "Scene.h"
#include "Action.h"
#include "ActionReceiver.h"
#include "CommonTypes.h"
#include "../resources/Resource.h"
#include "../resources/ResourceLib.h"
#include "../actions/KeyAction.h"
#include "../actions/TouchAction.h"
#include "../actions/TouchMoveAction.h"
#include "../actions/CursorAction.h"
#include "../actions/WindowAction.h"

class Window : public ActionReceiver
{
public:
	enum WindowState
	{
		WINDOWED,
		MINIMISED,
		MAXIMISED,
		FULLSCREEN
	};

	struct Config
	{
		Size2d Size;
		Position2d Position;
		WindowState State;
	};

public:
	Window(Scene& scene,
		ResourceLib& resourceLib);
	~Window();

private:
	LPTSTR _windowClass;	// Window Class
	HGLRC _rc;			// Rendering Context
	HDC	_dc;				// Device Context
	HWND _wnd;			// Window
	DWORD _style;
	Config _config;
	bool _resizing;
	bool _trackingMouse;
	unsigned int _buttonsDown;

	GlDrawContext _drawContext;
	Scene& _scene;
	ResourceLib& _resourceLib;

public:
	void SetWindowHandle(HWND wnd);
	void ShowMessage(LPCWSTR message);
	int Create(HINSTANCE hInstance, int nCmdShow);
	void AdjustSize();
	Config GetConfig() const;
	Size2d GetMinSize() const;
	bool IsFullscreen() const;
	bool IsResizing() const;
	void SetResizing(bool resizing);
	bool IsTrackingMouse() const;
	void SetTrackingMouse(bool resizing);
	void Resize(Size2d size, WindowState state);
	void SetWindowState(WindowState state);
	void Center();
	void Render();
	void Swap();
	void Release();

	virtual void OnAction(WindowAction winAction) override;
	virtual void OnAction(TouchAction touchAction) override;
	virtual void OnAction(TouchMoveAction touchAction) override;
	virtual void OnAction(KeyAction touchAction) override;

	static ATOM Register(HINSTANCE hInstance);
	static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept;

private:
	void LoadResources();
	void InitScene();

	static void InitStyle(WNDCLASSEX& wcex) noexcept;
	static void APIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void * userParam);
};