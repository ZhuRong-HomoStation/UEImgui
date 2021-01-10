#include "ImguiWindowWrapper.h"
#include "imgui.h"
#include "Widgets/SImguiWindow.h"

UImguiContext*		FImguiWindowWrapper::CurContext = nullptr;
UImguiInputAdapter*	FImguiWindowWrapper::CurInputAdapter = nullptr;

void FImguiWindowWrapper::SetupPlatformInterface(ImGuiPlatformIO* PlatformIO)
{
	PlatformIO->Platform_CreateWindow = UE_CreateWindow;
	PlatformIO->Platform_DestroyWindow = UE_DestroyWindow;
	PlatformIO->Platform_ShowWindow = UE_ShowWindow;
	PlatformIO->Platform_SetWindowPos = UE_SetWindowPos;
	PlatformIO->Platform_GetWindowPos = UE_GetWindowPos;
	PlatformIO->Platform_SetWindowSize = UE_SetWindowSize;
	PlatformIO->Platform_GetWindowSize = UE_GetWindowSize;
	PlatformIO->Platform_SetWindowFocus = UE_SetWindowFocus;
	PlatformIO->Platform_GetWindowFocus = UE_GetWindowFocus;
	PlatformIO->Platform_GetWindowMinimized = UE_GetWindowMinimized;
	PlatformIO->Platform_SetWindowTitle = UE_SetWindowTitle;
	PlatformIO->Platform_SetWindowAlpha = UE_SetWindowAlpha;
	PlatformIO->Platform_UpdateWindow = UE_UpdateWindow;
	PlatformIO->Platform_GetWindowDpiScale = UE_GetWindowDpiScale;
	PlatformIO->Platform_OnChangedViewport = UE_OnChangedViewport;
	PlatformIO->Platform_SetImeInputPos = UE_SetImeInputPos;
}

void FImguiWindowWrapper::UE_CreateWindow(ImGuiViewport* viewport)
{
	CurContext->_CreateWindow(viewport, CurInputAdapter);
}

void FImguiWindowWrapper::UE_DestroyWindow(ImGuiViewport* viewport)
{
	CurContext->_DestroyWindow(viewport);
}

void FImguiWindowWrapper::UE_ShowWindow(ImGuiViewport* viewport)
{
	CurContext->_ShowWindow(viewport);
}

void FImguiWindowWrapper::UE_UpdateWindow(ImGuiViewport* viewport)
{
	CurContext->_UpdateWindow(viewport);
}

ImVec2 FImguiWindowWrapper::UE_GetWindowPos(ImGuiViewport* viewport)
{
	return CurContext->_GetWindowPos(viewport);
}

void FImguiWindowWrapper::UE_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
{
	CurContext->_SetWindowPos(viewport, pos);
}

ImVec2 FImguiWindowWrapper::UE_GetWindowSize(ImGuiViewport* viewport)
{
	return CurContext->_GetWindowSize(viewport);
}

void FImguiWindowWrapper::UE_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
{
	CurContext->_SetWindowSize(viewport, size);
}

void FImguiWindowWrapper::UE_SetWindowFocus(ImGuiViewport* viewport)
{
	CurContext->_SetWindowFocus(viewport);
}

bool FImguiWindowWrapper::UE_GetWindowFocus(ImGuiViewport* viewport)
{
	return CurContext->_GetWindowFocus(viewport);
}

bool FImguiWindowWrapper::UE_GetWindowMinimized(ImGuiViewport* viewport)
{
	return CurContext->_GetWindowMinimized(viewport);
}

void FImguiWindowWrapper::UE_SetWindowTitle(ImGuiViewport* viewport, const char* title)
{
	CurContext->_SetWindowTitle(viewport, title);
}

void FImguiWindowWrapper::UE_SetWindowAlpha(ImGuiViewport* viewport, float alpha)
{
	CurContext->_SetWindowAlpha(viewport, alpha);
}

float FImguiWindowWrapper::UE_GetWindowDpiScale(ImGuiViewport* viewport)
{
	return CurContext->_GetWindowDpiScale(viewport);
}

void FImguiWindowWrapper::UE_OnChangedViewport(ImGuiViewport* viewport)
{
	CurContext->_OnChangedViewport(viewport);
}

void FImguiWindowWrapper::UE_SetImeInputPos(ImGuiViewport* viewport, ImVec2 pos)
{
	CurContext->_SetImeInputPos(viewport, pos);
}
