#include <dwmapi.h>
#include "DrawEng.h"
#include "proc_help.h"
#include "DrawCallBack.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include <d3d11.h>

#include <dinput.h>
#include <tchar.h>

#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "d3d11.lib")

ID3D11Device* g_pd3dDevice = NULL;
ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
IDXGISwapChain* g_pSwapChain = NULL;
ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
ImFont* font;
HWND myHWND;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

// Main code
void startDraw() {
	int clsLen = getRandomInt(100, 200);
	int wndLen = getRandomInt(100, 200);
	char* clsName = (char*)malloc(clsLen);
	char* wndName = (char*)malloc(wndLen);
	memset(clsName, 0, clsLen);
	memset(wndName, 0, wndLen);
	rand_str(clsName, clsLen - 1);
	rand_str(wndName, wndLen - 1);
	//printf("clsName: %s\nwndName: %s\n", clsName, wndName);
	WNDCLASSEXA wc = { sizeof(WNDCLASSEX), CS_HREDRAW, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL,clsName, NULL };
	RegisterClassExA(&wc);
	myHWND = CreateWindowExA(WS_EX_TOPMOST, wc.lpszClassName, wndName, WS_POPUP, 0, 0, 1920, 1080, NULL, NULL, GetModuleHandle(NULL), NULL);//创建窗口
	ShowWindow(myHWND, SW_SHOWDEFAULT);
	UpdateWindow(myHWND);
	SetWindowLongA(myHWND, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST);
	MARGINS marg = { -1 };
	DwmExtendFrameIntoClientArea(myHWND, &marg);
	if (!CreateDeviceD3D(myHWND)) {
		CleanupDeviceD3D();
		UnregisterClassA(wc.lpszClassName, wc.hInstance);
		return;
	}
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(myHWND);//imgui链接上这个窗口
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);//初始化d3d设备
	io.Fonts->AddFontDefault();
	font = io.Fonts->AddFontFromFileTTF(R"(c:\Windows\Fonts\msyhbd.ttc)", 25.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());//msyhbd
	IM_ASSERT(font != NULL);
	// Our state
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	const float _0f = 0.f;
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
	ImGui_ImplDX11_NewFrame();
	while (msg.message != WM_QUIT) {//窗口没有关闭
		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			continue;
		}
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();//新帧
		draw();
		ImGui::Render();//渲染

		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, &_0f);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		g_pSwapChain->Present(1, 0);//呈现
	}
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	DestroyWindow(myHWND);
	UnregisterClassA(wc.lpszClassName, wc.hInstance);
	return;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd) {
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	DEVMODE devMode;
	devMode.dmSize = sizeof(devMode);
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = devMode.dmDisplayFrequency;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2,
		D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel,
		&g_pd3dDeviceContext) != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D() {
	CleanupRenderTarget();
	if (g_pSwapChain) {
		g_pSwapChain->Release();
		g_pSwapChain = NULL;
	}
	if (g_pd3dDeviceContext) {
		g_pd3dDeviceContext->Release();
		g_pd3dDeviceContext = NULL;
	}
	if (g_pd3dDevice) {
		g_pd3dDevice->Release();
		g_pd3dDevice = NULL;
	}
}

void CreateRenderTarget() {
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget() {
	if (g_mainRenderTargetView) {
		g_mainRenderTargetView->Release();
		g_mainRenderTargetView = NULL;
	}
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

RECT getGameWindowInfo(HWND hWnd, RECT recinfo) {
	POINT local = { 0, 0 };
	ClientToScreen(hWnd, &local);

	GetClientRect(hWnd, &recinfo);

	recinfo.left = local.x;
	recinfo.top = local.y;
	return recinfo;
}

void drawStrockText(ImDrawList* drawList, ImFont* font, float size, ImVec2 pos, ImColor col, const char* text) {
	ImColor strockcol = IM_COL32(0.0f, 0.0f, 0.0f, 255.0f);
	ImVec2 xL = pos;
	ImVec2 xA = pos;
	ImVec2 yL = pos;
	ImVec2 yA = pos;
	xL.x = xL.x - 1;
	xA.x = xA.x + 1;
	yL.y = yL.y - 1;
	yA.y = yA.y + 1;
	drawList->AddText(font, size, xL, strockcol, text, text + strlen(text), 0.f);
	drawList->AddText(font, size, xA, strockcol, text, text + strlen(text), 0.f);
	drawList->AddText(font, size, yL, strockcol, text, text + strlen(text), 0.f);
	drawList->AddText(font, size, yA, strockcol, text, text + strlen(text), 0.f);
	drawList->AddText(font, size, pos, col, text, text + strlen(text), 0.f);
}

void drawRectFilled(ImDrawList* drawList, ImVec4 rect, ImColor col) {
	ImVec2 xy = { 0.f, 0.f };
	xy.x = rect.x;
	xy.y = rect.y;

	ImVec2 zw = { 0.f, 0.f };
	zw.x = xy.x + rect.z;
	zw.y = xy.y + rect.w;

	drawList->AddRectFilled(xy, zw, col);
	drawList->AddRect(xy, zw, col);
}
/*绘制*/
void drawFrame(ImDrawList* drawList, ImVec4 rect, float px, ImColor col) {
	drawLine(drawList, { rect.x, rect.y, rect.x + rect.z / 4, rect.y }, px, col);
	drawLine(drawList, { rect.x, rect.y, rect.x, rect.y + rect.w / 4 }, px, col);
	drawLine(drawList, { rect.x + rect.z, rect.y, rect.x + rect.z - rect.z / 4, rect.y }, px, col);
	drawLine(drawList, { rect.x + rect.z, rect.y, rect.x + rect.z, rect.y + rect.w / 4 }, px, col);
	drawLine(drawList, { rect.x, rect.y + rect.w, rect.x + rect.z / 4, rect.y + rect.w }, px, col);
	drawLine(drawList, { rect.x, rect.y + rect.w, rect.x, rect.y + rect.w - rect.w / 4 }, px, col);
	drawLine(drawList, { rect.x + rect.z, rect.y + rect.w, rect.x + rect.z - rect.z / 4, rect.y + rect.w }, px, col);
	drawLine(drawList, { rect.x + rect.z, rect.y + rect.w, rect.x + rect.z, rect.y + rect.w - rect.w / 4 }, px, col);
}

void drawLine(ImDrawList* drawList, ImVec4 rect, float px, ImColor col) {
	float sx = rect.x + gameRect.left;
	float sy = rect.y + gameRect.top;
	float ex = rect.z + gameRect.left;
	float ey = rect.w + gameRect.top;
	drawList->AddLine({ sx, sy }, { ex, ey }, col, px);
}