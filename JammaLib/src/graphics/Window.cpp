///////////////////////////////////////////////////////////
//
// Copyright(c) 2018-2019 Matt Jones
// Subject to the MIT license, see LICENSE file.
//
///////////////////////////////////////////////////////////

#include "Window.h"
#include "StringUtils.h"

///////////////////////////////////////////////////////////

Window::Window(Scene& scene) :
	_scene(scene),
	_style(WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
{
	_config.Width = scene.Width();
	_config.Height = scene.Height();
	_config.PosX = CW_USEDEFAULT;
	_config.PosY = 0;
	_config.Windowed = true;
}

Window::~Window()
{
}

void Window::InitScene()
{
	_scene.Init();
}

void Window::ShowMessage(LPCWSTR message)
{
	MessageBox(0, message, L"Window::create", MB_ICONERROR);
}

int Window::Create(HINSTANCE hInstance, int nCmdShow)
{
	_windowClass = MAKEINTATOM(RegisterClass(hInstance));
	if (_windowClass == 0) {
		ShowMessage(L"registerClass() failed.");
		return 1;
	}

	// Temporary window creation to get supported formats
	HWND fakeWND = CreateWindow(
		_windowClass, L"Fake Window",
		_style,
		0, 0,						// Position x, y
		1, 1,						// width, height
		NULL, NULL,					// parent window, menu
		hInstance, NULL);			// instance, param

	HDC fakeDC = GetDC(fakeWND);	// Device Context

	PIXELFORMATDESCRIPTOR fakePFD;
	ZeroMemory(&fakePFD, sizeof(fakePFD));
	fakePFD.nSize = sizeof(fakePFD);
	fakePFD.nVersion = 1;
	fakePFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	fakePFD.iPixelType = PFD_TYPE_RGBA;
	fakePFD.cColorBits = 32;
	fakePFD.cAlphaBits = 8;
	fakePFD.cDepthBits = 24;

	const int fakePFDID = ChoosePixelFormat(fakeDC, &fakePFD);
	if (fakePFDID == 0) {
		ShowMessage(L"ChoosePixelFormat() failed.");
		return 1;
	}

	if (SetPixelFormat(fakeDC, fakePFDID, &fakePFD) == false) {
		ShowMessage(L"SetPixelFormat() failed.");
		return 1;
	}

	HGLRC fakeRC = wglCreateContext(fakeDC);	// Rendering Contex

	if (fakeRC == 0) {
		ShowMessage(L"wglCreateContext() failed.");
		return 1;
	}

	if (wglMakeCurrent(fakeDC, fakeRC) == false) {
		ShowMessage(L"wglMakeCurrent() failed.");
		return 1;
	}

	// get pointers to functions (or init opengl loader here)
	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
	wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
	if (wglChoosePixelFormatARB == nullptr)
	{
		ShowMessage(L"wglGetProcAddress() failed.");
		return 1;
	}

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
	wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
	if (wglCreateContextAttribsARB == nullptr)
	{
		ShowMessage(L"wglGetProcAddress() failed.");
		return 1;
	}

	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;
	wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
	if (wglSwapIntervalEXT == nullptr)
	{
		ShowMessage(L"wglGetProcAddress() failed.");
		return 1;
	}

	if (_config.Windowed == true)
	{
		AdjustSize();
		Center();
	}

	// create a new window and context
	_wnd = CreateWindow(
		_windowClass, L"OpenGL Window",	// class name, window name
		_style,							// styles
		_config.PosX, _config.PosY,		// posx, posy. If x is set to CW_USEDEFAULT y is ignored
		_config.Width, _config.Height,	// width, height
		NULL, NULL,						// parent window, menu
		hInstance, NULL);				// instance, param

	_dc = GetDC(_wnd);

	const int pixelAttribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_ALPHA_BITS_ARB, 8,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
		WGL_SAMPLES_ARB, 4,
		0
	};

	int pixelFormatID; UINT numFormats;
	const bool status = wglChoosePixelFormatARB(_dc, pixelAttribs, NULL, 1, &pixelFormatID, &numFormats);

	if (status == false || numFormats == 0)
	{
		ShowMessage(L"wglChoosePixelFormatARB() failed.");
		return 1;
	}

	PIXELFORMATDESCRIPTOR PFD;
	DescribePixelFormat(_dc, pixelFormatID, sizeof(PFD), &PFD);
	SetPixelFormat(_dc, pixelFormatID, &PFD);

	const int major_min = 4, minor_min = 0;
	const int contextAttribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, major_min,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor_min,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
//		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
		0
	};

	_rc = wglCreateContextAttribsARB(_dc, 0, contextAttribs);
	if (_rc == NULL) {
		ShowMessage(L"wglCreateContextAttribsARB() failed.");
		return 1;
	}

	// Delete temporary context and window
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(fakeRC);
	ReleaseDC(fakeWND, fakeDC);
	DestroyWindow(fakeWND);
	if (!wglMakeCurrent(_dc, _rc))
	{
		ShowMessage(L"wglMakeCurrent() failed.");
		return 1;
	}

	wglSwapIntervalEXT(1);

	// Init opengl loader here (extra safe version)
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		auto errStr = (const char*)glewGetErrorString(err);
		ShowMessage(CharsToUnicodeString(errStr));
		return 1;
	}

	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(Window::MessageCallback, 0);
	
	glEnable(GL_DEPTH_TEST);

	InitScene();

	const char* glVersion = (const char *)glGetString(GL_VERSION);
	size_t size = strlen(glVersion) + 1;
	wchar_t* glVersionStr = new wchar_t[size];

	size_t outSize;
	mbstowcs_s(&outSize, glVersionStr, size, glVersion, size - 1);
	SetWindowText(_wnd, glVersionStr);
	ShowWindow(_wnd, nCmdShow);

	return 0;
}

ATOM Window::RegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WindowProcedure;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = TEXT("Core");

	return RegisterClassEx(&wcex);
}

void Window::AdjustSize()
{
	RECT rect = { 0, 0, _config.Width, _config.Height };
	AdjustWindowRect(&rect, _style, false);
	_config.Width = rect.right - rect.left;
	_config.Height = rect.bottom - rect.top;
}

void Window::Center()
{
	RECT primaryDisplaySize;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &primaryDisplaySize, 0);
	_config.PosX = (primaryDisplaySize.right - _config.Width) / 2;
	_config.PosY = (primaryDisplaySize.bottom - _config.Height) / 2;
}

void Window::Render()
{
	glClearColor(0.129f, 0.586f, 0.949f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_scene.Draw(_drawContext);
}

void Window::Swap()
{
	SwapBuffers(_dc);
}

void Window::Destroy()
{
	wglMakeCurrent(NULL, NULL);

	if (_rc)
		wglDeleteContext(_rc);

	if (_dc)
		ReleaseDC(_wnd, _dc);

	if (_wnd)
		DestroyWindow(_wnd);
}

Window::Config Window::GetConfig()
{
	return _config;
}

void APIENTRY Window::MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

///////////////////////////////////////////////////////////

LRESULT CALLBACK Window::WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
				PostQuitMessage(0);

			break;
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;		// message handled
}
