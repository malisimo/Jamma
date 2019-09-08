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

#define MAX_JSON_CHARS 1000000u

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
	std::wstring initPath = GetPath(PATH_ROAMING) + L"/Jamma/defaults.json";
	io::TextReadWriter txtFile;
	auto res = txtFile.Read(initPath, MAX_JSON_CHARS);

	std::string iniTxt = "{\"rig\":\"\",\"jam\":\"default.jam\",\"jamload\":1,\"rigload\":0,\"win\":[-0,0,1400,1000]}";
	if (!res.has_value())
		txtFile.Write(initPath, iniTxt, (unsigned int)iniTxt.size(), 0);
	else
	{
		auto [ini, numChars, unused] = res.value();
		iniTxt = ini;
	}

	std::stringstream ss(iniTxt);
	return InitFile::FromStream(std::move(ss));
}

std::optional<io::JamFile> LoadJam(io::InitFile& ini)
{
	io::TextReadWriter txtFile;

	std::string jamJson = "{\"name\":\"default\",\"stations\":[{\"name\":\"Mic1\",\"stationtype\":0,\"takes\":[{\"name\":\"Take1\",\"loops\":[{\"name\":\"Loop1.wav\",\"length\":400000,\"mix\":{\"type\":\"pan\",\"chans\":[0.5,0.5]}}]}]}";
	auto res = txtFile.Read(ini.Jam, MAX_JSON_CHARS);
	if (!res.has_value())
	{
		ini.Jam = GetPath(PATH_ROAMING) + L"/Jamma/default.jam";
		txtFile.Write(ini.Jam,
			jamJson,
			(unsigned int)jamJson.size(),
			0);
	}
	else
	{
		auto [str, numChars, unused] = res.value();
		jamJson = str;
	}

	std::stringstream ss(jamJson);
	return JamFile::FromStream(std::move(ss));
}

std::optional<io::RigFile> LoadRig(io::InitFile& ini)
{
	io::TextReadWriter txtFile;

	std::string rigJson = "{\"name\":\"default\",\"audio\":{\"name\":\"default\",\"bufsize\":512,\"numchannelsin\":2,\"numchannelsout\":2},\"triggers\":[{\"name\":\"Trig1\",\"stationtype\":0,\"pairs\":[{\"activatedown\":49,\"activateup\":49,\"ditchdown\":50,\"ditchup\":50}],\"input\":[0,1]}";
	auto res = txtFile.Read(ini.Rig, MAX_JSON_CHARS);
	if (!res.has_value())
	{
		ini.Rig = GetPath(PATH_ROAMING) + L"/Jamma/default.rig";
		txtFile.Write(ini.Rig,
			rigJson,
			(unsigned int)rigJson.size(),
			0);
	}
	else
	{
		auto [str, numChars, unused] = res.value();
		rigJson = str;
	}

	std::stringstream ss(rigJson);
	return RigFile::FromStream(std::move(ss));
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
		auto jamOpt = LoadJam(defaults.value());
		if (jamOpt.has_value())
			jam = jamOpt.value();

		auto rigOpt = LoadRig(defaults.value());
		if (rigOpt.has_value())
			rig = rigOpt.value();
	}

	auto scene = Scene::FromFile(sceneParams, jam, rig, utils::GetParentDirectory(defaults.value().Jam));
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
				scene.value()->CloseAudio();
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
