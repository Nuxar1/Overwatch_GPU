#pragma once

#include <unordered_map>
#include <Windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <Dwmapi.h> 
#include <TlHelp32.h>
#include <future>
#include <time.h>

class Overlay {
private:
	int overlayPosX = 0;
	int overlayPosY = 0;
	MSG message;
public:
	std::chrono::high_resolution_clock::time_point t1, t2;

	void xInitD3d();


	void MainLoop();
	HWND Window;
	//Constructor
	Overlay();
	~Overlay();

	void ChangeClickability();

	void xCreateWindow();

	int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	void Loop();
	void ImGuiInit();
};
namespace winKey
{
	extern std::unordered_map<int, std::string> KeyNames;
	std::string keyTextForInt(int keyVal);
	void saveHotKey(int* hotkey);
};