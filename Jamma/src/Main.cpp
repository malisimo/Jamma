///////////////////////////////////////////////////////////
//
// Copyright(c) 2018-2019 Matt Jones
// Subject to the MIT license, see LICENSE file.
//
///////////////////////////////////////////////////////////

#include "Main.h"
#include "Window.h"
#include <vector>
#include <fstream>
#include <sstream>

using namespace engine;
using namespace base;
using namespace resources;
using namespace graphics;
using namespace utils;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	AllocConsole();
	FILE* newStdout = nullptr;
	FILE* newStderr = nullptr;
	FILE* newStdin = nullptr;
	freopen_s(&newStdout, "CONOUT$", "w", stdout);
	freopen_s(&newStderr, "CONOUT$", "w", stderr);
	freopen_s(&newStdin, "CONIN$", "r", stdin);

	Scene scene(SceneParams(DrawableParams{ "" }, SizeableParams{ 1400, 1000 }));
	ResourceLib resourceLib;
	Window window(scene, resourceLib);

	if (window.Create(hInstance, nCmdShow) != 0)
		PostQuitMessage(1);

	scene.InitAudio();

	MSG msg;
	bool active = true;
	while (active)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				active = false;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		window.Render();
		window.Swap();
	}

	window.Release();

	return (int)msg.wParam;
}

