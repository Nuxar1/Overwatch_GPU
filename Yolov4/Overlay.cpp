

#include "Overlay.h"
#include <iostream>
#include "Globals.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
//#include "legacyfont.h"
#include <shellapi.h>
#include "xor.hpp"
#include "tahoma.ttf.h"


using namespace::std;

Overlay::Overlay() {}

Overlay::~Overlay()
{
}


// Direct 11
static ID3D11Device* g_pd3dDevice = 0;
static ID3D11DeviceContext* g_pd3dDeviceContext = 0;
static IDXGISwapChain* g_pSwapChain = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = 0;



void CheatMenuInit()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.00f;
	style.WindowPadding = ImVec2(20, 14);
	style.WindowMinSize = ImVec2(480, 450);
	style.WindowRounding = 0.0f; //4.0 for slight curve
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.ChildRounding = 0.0f;
	style.FramePadding = ImVec2(4, 3);
	style.FrameRounding = 0.0f; //2.0
	style.ItemSpacing = ImVec2(8, 4);
	style.ItemInnerSpacing = ImVec2(4, 4);
	style.TouchExtraPadding = ImVec2(0, 0);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 3.0f;
	style.ScrollbarSize = 6.0f;
	style.ScrollbarRounding = 1; //16.0
	style.GrabMinSize = 0.1f;
	style.GrabRounding = 16.0f; //16.00
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.DisplayWindowPadding = ImVec2(22, 22);
	style.DisplaySafeAreaPadding = ImVec2(4, 4);
	style.AntiAliasedLines = true;
	style.CurveTessellationTol = 1.25f;
	// Setup style
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 1);
	colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.37f, 0.14f, 0.14f, 0.67f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.39f, 0.20f, 0.20f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.48f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.48f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.56f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.19f, 0.19f, 0.50f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.89f, 0.00f, 0.19f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(1.00f, 0.19f, 0.19f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.80f, 0.17f, 0.00f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.89f, 0.00f, 0.19f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.33f, 0.35f, 0.36f, 0.53f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.76f, 0.28f, 0.44f, 0.67f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.47f, 0.47f, 0.47f, 0.67f);
	colors[ImGuiCol_Separator] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
	colors[ImGuiCol_Tab] = ImVec4(0.07f, 0.07f, 0.07f, 0.51f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.86f, 0.23f, 0.43f, 0.67f);
	colors[ImGuiCol_TabActive] = ImVec4(0.19f, 0.19f, 0.19f, 0.57f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.05f, 0.05f, 0.05f, 0.90f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.13f, 0.13f, 0.74f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}


int CurTabOpen = 0;
void AddTab(size_t Index, const char* Text)
{
	static const size_t TabWidth = 122;
	static const size_t TabHeight = 30;

	ImGui::PushID(Index);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);

	if (Index == 1)
		ImGui::SameLine(Index * (TabWidth + 16));
	else if (Index > 1)
		ImGui::SameLine(Index * (TabWidth + 4 - Index));

	if (CurTabOpen == Index)
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(15, 15, 15));			// Color on tab open
	else
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(30, 30, 30));			// Color on tab closed

	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(40, 40, 40));			// Color on mouse hover in tab
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(35, 35, 35));			// Color on click tab

	if (ImGui::Button(Text, ImVec2(TabWidth, TabHeight)))	// If tab clicked
		CurTabOpen = Index;

	ImGui::PopStyleVar();
	ImGui::PopStyleColor(3);
	ImGui::PopID();
}

bool streamProof = true;
ImFont* m_pTahomaFont;
void DrawMenu()
{
	CheatMenuInit();

	ImGui::PushFont(m_pTahomaFont);
	ImGui::SetNextWindowSize(ImVec2(480, 520));
	ImGui::SetNextWindowBgAlpha(0.95f);
	ImGui::Begin(_xor_("Zambu GPU Aimbot").c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize);


	ImGui::Spacing();

	ImGui::SliderFloat(_xor_("Confidence").c_str(), &Settings::CONFIDENCE_THRESHOLD, 0, 1);
	ImGui::SliderFloat(_xor_("NMS threshold").c_str(), &Settings::NMS_THRESHOLD, 0, 1);
	ImGui::SliderFloat(_xor_("Speed").c_str(), &Settings::speed, 0, 15);
	ImGui::SliderFloat(_xor_("Flick speed").c_str(), &Settings::flickSpeed, 0, 5000);
	ImGui::SliderFloat(_xor_("Horizontal offset").c_str(), &Settings::horizontal_offset, 0, 1);
	ImGui::SliderFloat(_xor_("Vertical offset").c_str(), &Settings::vertical_offset, 0, 1);
	ImGui::SliderInt(_xor_("Fov X").c_str(), &(Settings::input_dimensions[0]), 0, Settings::screen_dimensions[0]);
	ImGui::SliderInt(_xor_("Fov Y").c_str(), &(Settings::input_dimensions[1]), 0, Settings::screen_dimensions[1]);
	ImGui::Checkbox(_xor_("Degub window").c_str(), &Settings::degub);

	if (ImGui::Checkbox(_xor_("Stream Proof").c_str(), &streamProof))
	{
		if (streamProof)
		{
			SetWindowDisplayAffinity(Globals::Over.Window, WDA_EXCLUDEFROMCAPTURE);
		}
		else
		{
			SetWindowDisplayAffinity(Globals::Over.Window, WDA_NONE);
		}
	}
	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		ImGui::Text(_xor_("If this is turned on the cheat overlay will be black (Windows 1909 or lower) or transparent (Windows 2004 or higher).").c_str());
		ImGui::EndTooltip();
	}

	ImGui::End();

	ImGui::PopFont();
}

void Overlay::Loop() {

	//ImGui Init
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
	CheatMenuInit();
	UpdateWindow(Window);
	SetForegroundWindow(Window);

	//ImGui fonts
	ImGuiIO& io = ImGui::GetIO();
	m_pTahomaFont = io.Fonts->AddFontFromMemoryTTF(tahoma_ttf, sizeof(tahoma_ttf), 18.0f, nullptr,
		io.Fonts->GetGlyphRangesCyrillic());

	MSG message;
	ZeroMemory(&message, sizeof(message));

	using clock = std::chrono::high_resolution_clock;
	clock::time_point t1, t2;
	double frametime = 144;
	//Main game loop

	while (true)
	{
		t1 = clock::now();


		if (PeekMessage(&message, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
			continue;
		}



		else
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			if (Globals::Menu)
			{
				DrawMenu();
			}


			float clearColor[] = { 0, 0, 0, 0 };

			ImGui::EndFrame();

			g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, reinterpret_cast<const float*>(clearColor));
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			g_pSwapChain->Present(0, 0);

			HWND DESKTOP = GetForegroundWindow();
			HWND MOVERDESK = GetWindow(DESKTOP, GW_HWNDPREV);
			SetWindowPos(Window, MOVERDESK, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);
			UpdateWindow(Window);
		}

		while (std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - t1).count() < 5000)
			Sleep(1);

	}
}

#pragma region CreateWindow
void Overlay::ImGuiInit() {
	//ImGui Init
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
	CheatMenuInit();
	UpdateWindow(Window);
	SetForegroundWindow(Window);


	ZeroMemory(&message, sizeof(message));
}
void CreateRenderTarget()
{
	DXGI_SWAP_CHAIN_DESC sd;
	g_pSwapChain->GetDesc(&sd);

	ID3D11Texture2D* pBackBuffer;
	D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
	ZeroMemory(&render_target_view_desc, sizeof(render_target_view_desc));
	render_target_view_desc.Format = sd.BufferDesc.Format;
	render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, &render_target_view_desc, &g_mainRenderTargetView);
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView)
	{
		g_mainRenderTargetView->Release();
		g_mainRenderTargetView = NULL;
	}
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain)
	{
		g_pSwapChain->Release();
		g_pSwapChain = NULL;
	}
	if (g_pd3dDeviceContext)
	{
		g_pd3dDeviceContext->Release();
		g_pd3dDeviceContext = NULL;
	}
	if (g_pd3dDevice)
	{
		g_pd3dDevice->Release();
		g_pd3dDevice = NULL;
	}
}

void Overlay::xInitD3d()
{
	//printf("xInitD3d\n");
	DXGI_SWAP_CHAIN_DESC sd;
	{
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 2;
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 0;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = Window;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[1] = { D3D_FEATURE_LEVEL_11_0, };
	//XGUARD_HR(
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 1,
		D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);// != S_OK);

#if 0
	D3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	D3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	D3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	D3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
#endif

	CreateRenderTarget();
	//XGUARD_WIN(SetWindowDisplayAffinity(Window, WDA_MONITOR));
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK xWindowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
		return true;

	switch (uMsg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX11_InvalidateDeviceObjects();
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
			ImGui_ImplDX11_CreateDeviceObjects();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		exit(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void Overlay::ChangeClickability()
{
	long style = GetWindowLong(Window, GWL_EXSTYLE);
	if (Globals::Menu) {
		style &= ~WS_EX_LAYERED;
		SetWindowLong(Window, GWL_EXSTYLE, style);
		SetForegroundWindow(Window);
		//windowstate = 1;
	}
	else {
		style |= WS_EX_LAYERED;
		SetWindowLong(Window, GWL_EXSTYLE, style);
		//windowstate = 0;
	}
}


char genRandom()
{
	return alphanum[rand() % stringLength];
}



void Overlay::xCreateWindow()
{

	srand(time(0));
	std::wstring Str = s2ws(_xor_("DirectX Screen Capture").c_str());
	WNDCLASSEX windowClass;

	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = xWindowProc;
	windowClass.hInstance = GetModuleHandle(0);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)RGB(0, 0, 0);


	windowClass.lpszClassName = Str.c_str();

	RegisterClassEx(&windowClass);

	Window = CreateWindowEx(NULL, windowClass.lpszClassName, Str.c_str(), WS_POPUP | WS_VISIBLE, 0, 0, ScreenWidth, ScreenHeight, NULL, NULL, windowClass.hInstance, NULL);
	SetWindowLong(Window, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT);

	SetWindowDisplayAffinity(Window, WDA_EXCLUDEFROMCAPTURE);

	const MARGINS margins = { -1 };
	DwmExtendFrameIntoClientArea(Window, &margins);

	ShowWindow(Window, SW_SHOW);
	UpdateWindow(Window);
	xInitD3d();
}
#pragma endregion


namespace winKey
{
	std::unordered_map<int, std::string> KeyNames = {
		{ VK_LBUTTON, "VK_LBUTTON" },
		{ VK_RBUTTON, "VK_RBUTTON" },
		{ VK_CANCEL, "VK_CANCEL" },
		{ VK_MBUTTON, "VK_MBUTTON" },
		{ VK_XBUTTON1, "VK_XBUTTON1" },
		{ VK_XBUTTON2, "VK_XBUTTON2" },
		{ VK_BACK, "VK_BACK" },
		{ VK_TAB, "VK_TAB" },
		{ VK_CLEAR, "VK_CLEAR" },
		{ VK_RETURN, "VK_RETURN" },
		{ VK_SHIFT, "VK_SHIFT" },
		{ VK_CONTROL, "VK_CONTROL" },
		{ VK_MENU, "VK_MENU" },
		{ VK_PAUSE, "VK_PAUSE" },
		{ VK_CAPITAL, "VK_CAPITAL" },
		{ VK_KANA, "VK_KANA" },
		{ VK_JUNJA, "VK_JUNJA" },
		{ VK_FINAL, "VK_FINAL" },
		{ VK_KANJI, "VK_KANJI" },
		{ VK_ESCAPE, "VK_ESCAPE" },
		{ VK_CONVERT, "VK_CONVERT" },
		{ VK_NONCONVERT, "VK_NONCONVERT" },
		{ VK_ACCEPT, "VK_ACCEPT" },
		{ VK_MODECHANGE, "VK_MODECHANGE" },
		{ VK_SPACE, "VK_SPACE" },
		{ VK_PRIOR, "VK_PRIOR" },
		{ VK_NEXT, "VK_NEXT" },
		{ VK_END, "VK_END" },
		{ VK_HOME, "VK_HOME" },
		{ VK_LEFT, "VK_LEFT" },
		{ VK_UP, "VK_UP" },
		{ VK_RIGHT, "VK_RIGHT" },
		{ VK_DOWN, "VK_DOWN" },
		{ VK_SELECT, "VK_SELECT" },
		{ VK_PRINT, "VK_PRINT" },
		{ VK_EXECUTE, "VK_EXECUTE" },
		{ VK_SNAPSHOT, "VK_SNAPSHOT" },
		{ VK_INSERT, "VK_INSERT" },
		{ VK_DELETE, "VK_DELETE" },
		{ VK_HELP, "VK_HELP" },
		{ 0x30  , "0" },
		{ 0x31 , "1" },
		{ 0x32 , "2" },
		{ 0x33 , "3" },
		{ 0x34 , "4" },
		{ 0x35 , "5" },
		{ 0x36 , "6" },
		{ 0x37 , "7" },
		{ 0x38 , "8" },
		{ 0x39 , "9" },
		{ 0x41 , "A" },
		{ 0x42 , "B" },
		{ 0x43 , "C" },
		{ 0x44 , "D" },
		{ 0x45 , "E" },
		{ 0x46 , "F" },
		{ 0x47 , "G" },
		{ 0x48 , "H" },
		{ 0x49 , "I" },
		{ 0x4A , "J" },
		{ 0x4B , "K" },
		{ 0x4C , "L" },
		{ 0x4D , "M" },
		{ 0x4E , "N" },
		{ 0x4F , "O" },
		{ 0x50 , "P" },
		{ 0x51 , "Q" },
		{ 0x52 , "R" },
		{ 0x53 , "S" },
		{ 0x54 , "T" },
		{ 0x55 , "U" },
		{ 0x56 , "V" },
		{ 0x57 , "W" },
		{ 0x58 , "X" },
		{ 0x59 , "Y" },
		{ 0x5A , "Z" },
		{ VK_LWIN, "VK_LWIN" },
		{ VK_RWIN, "VK_RWIN" },
		{ VK_APPS, "VK_APPS" },
		{ VK_SLEEP, "VK_SLEEP" },
		{ VK_NUMPAD0, "VK_NUMPAD0" },
		{ VK_NUMPAD1, "VK_NUMPAD1" },
		{ VK_NUMPAD2, "VK_NUMPAD2" },
		{ VK_NUMPAD3, "VK_NUMPAD3" },
		{ VK_NUMPAD4, "VK_NUMPAD4" },
		{ VK_NUMPAD5, "VK_NUMPAD5" },
		{ VK_NUMPAD6, "VK_NUMPAD6" },
		{ VK_NUMPAD7, "VK_NUMPAD7" },
		{ VK_NUMPAD8, "VK_NUMPAD8" },
		{ VK_NUMPAD9, "VK_NUMPAD9" },
		{ VK_MULTIPLY, "VK_MULTIPLY" },
		{ VK_ADD, "VK_ADD" },
		{ VK_SEPARATOR, "VK_SEPARATOR" },
		{ VK_SUBTRACT, "VK_SUBTRACT" },
		{ VK_DECIMAL, "VK_DECIMAL" },
		{ VK_DIVIDE, "VK_DIVIDE" },
		{ VK_F1, "VK_F1" },
		{ VK_F2, "VK_F2" },
		{ VK_F3, "VK_F3" },
		{ VK_F4, "VK_F4" },
		{ VK_F5, "VK_F5" },
		{ VK_F6, "VK_F6" },
		{ VK_F7, "VK_F7" },
		{ VK_F8, "VK_F8" },
		{ VK_F9, "VK_F9" },
		{ VK_F10, "VK_F10" },
		{ VK_F11, "VK_F11" },
		{ VK_F12, "VK_F12" },
		{ VK_F13, "VK_F13" },
		{ VK_F14, "VK_F14" },
		{ VK_F15, "VK_F15" },
		{ VK_F16, "VK_F16" },
		{ VK_F17, "VK_F17" },
		{ VK_F18, "VK_F18" },
		{ VK_F19, "VK_F19" },
		{ VK_F20, "VK_F20" },
		{ VK_F21, "VK_F21" },
		{ VK_F22, "VK_F22" },
		{ VK_F23, "VK_F23" },
		{ VK_F24, "VK_F24" },
		{ VK_NUMLOCK, "VK_NUMLOCK" },
		{ VK_SCROLL, "VK_SCROLL" },
		{ VK_OEM_NEC_EQUAL, "VK_OEM_NEC_EQUAL" },
		{ VK_OEM_FJ_MASSHOU, "VK_OEM_FJ_MASSHOU" },
		{ VK_OEM_FJ_TOUROKU, "VK_OEM_FJ_TOUROKU" },
		{ VK_OEM_FJ_LOYA, "VK_OEM_FJ_LOYA" },
		{ VK_OEM_FJ_ROYA, "VK_OEM_FJ_ROYA" },
		{ VK_LSHIFT, "VK_LSHIFT" },
		{ VK_RSHIFT, "VK_RSHIFT" },
		{ VK_LCONTROL, "VK_LCONTROL" },
		{ VK_RCONTROL, "VK_RCONTROL" },
		{ VK_LMENU, "VK_LMENU" },
		{ VK_RMENU, "VK_RMENU" }
	};

	std::string keyTextForInt(int keyVal) {
		std::unordered_map<int, std::string>::const_iterator got = KeyNames.find(keyVal);
		if (got != KeyNames.end()) {
			return KeyNames[keyVal];
		}
		return _xor_("Unknown");
	}

	void saveHotKey(int* hotkey) {
		int newKey = 0;
		Sleep(500);
		while (newKey == 0) {
			Sleep(10);
			for (int i = 1; i <= 255; ++i) {
				if (GetAsyncKeyState(i) & 0x8000) {
					newKey = i;
					*hotkey = newKey;
					return;
				}
			}
		}
	}
}