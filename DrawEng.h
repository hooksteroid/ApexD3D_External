#pragma once

#include <dinput.h>
#include <tchar.h>
#include <d3d11.h>
#include "imgui/imgui.h"

// Data
extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dDeviceContext;
extern IDXGISwapChain* g_pSwapChain;
extern ID3D11RenderTargetView* g_mainRenderTargetView;
extern ImFont* font;

void startDraw();

RECT getGameWindowInfo(HWND hWnd, RECT recinfo);

void drawStrockText(ImDrawList* drawList, ImFont* font, float size, ImVec2 pos, ImColor col, const char* text);

void drawRectFilled(ImDrawList* drawList, ImVec4 rect, ImColor col);

void drawFrame(ImDrawList* drawList, ImVec4 rect, float px, ImColor col);

void drawLine(ImDrawList* drawList, ImVec4 rect, float px, ImColor col);