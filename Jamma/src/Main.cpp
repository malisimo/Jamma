///////////////////////////////////////////////////////////
//
// Copyright(c) 2018-2019 Matt Jones
// Subject to the MIT license, see LICENSE file.
//
///////////////////////////////////////////////////////////

#include "Main.h"
#include "Window.h"
#include "PathUtils.h"
#include "../io/TextReadWriter.h"
#include "../io/InitFile.h"
#include <vector>
#include <fstream>
#include <sstream>

using namespace engine;
using namespace base;
using namespace resources;
using namespace graphics;
using namespace utils;
using namespace io;

#define MAX_JSON_CHARS 1E6

void SetupConsole()
{
	AllocConsole();
	FILE* newStdout = nullptr;
	FILE* newStderr = nullptr;
	FILE* newStdin = nullptr;
	freopen_s(&newStdout, "CONOUT$", "w", stdout);
	freopen_s(&newStderr, "CONOUT$", "w", stderr);
	freopen_s(&newStdin, "CONIN$", "r", stdin);
}

std::optional<io::InitFile> LoadIni()
{
	std::wstring initPath = GetPath(PATH_ROAMING) + L"/defaults.json";
	io::TextReadWriter txtFile;
	auto res = txtFile.Read(initPath, MAX_JSON_CHARS);

	std::string iniTxt = "{\"rig\":\"\",\"jam\":\"empty.jam\",\"jamload\":1,\"rigload\":0,\"win\":[-0,0,1400,1000]}";
	if (!res.has_value())
		txtFile.Write(initPath, iniTxt, iniTxt.size(), 0);
	else
	{
		auto [ini, numChars, unused] = res.value();
		iniTxt = ini;
	}

	std::stringstream ss(iniTxt);
	return InitFile::FromStream(std::move(ss));
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	SetupConsole();

	auto defaults = LoadIni();

	SceneParams sceneParams(DrawableParams{ "" }, SizeableParams{ 1400, 1000 });
	JamFile jam;
	RigFile rig;

	if (defaults.has_value())
	{
		io::TextReadWriter txtFile;

		auto res = txtFile.Read(defaults.value().Jam, MAX_JSON_CHARS);
		if (res.has_value())
		{
			auto [str, numChars, unused] = res.value();

			std::stringstream ss(str);
			auto opt = JamFile::FromStream(std::move(ss));
		
			if (opt.has_value())
				jam = opt.value();
		}

		res = txtFile.Read(defaults.value().Rig, MAX_JSON_CHARS);
		if (res.has_value())
		{
			auto [str, numChars, unused] = res.value();

			std::stringstream ss(str);
			auto opt = RigFile::FromStream(std::move(ss));

			if (opt.has_value())
				rig = opt.value();
		}
	}

	auto scene = Scene::FromFile(sceneParams, jam, rig);

	if (!scene.has_value())
	{
		std::cout << "Failed to create Scene... quitting" << std::endl;
		return -1;
	}

	ResourceLib resourceLib;
	Window window(*(scene.value()), resourceLib);

	if (window.Create(hInstance, nCmdShow) != 0)
		PostQuitMessage(1);

	scene.value()->InitAudio();

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

