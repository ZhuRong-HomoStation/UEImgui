#pragma once
#include "CoreMinimal.h"
#include "imgui.h"

class UImguiContext;
class UImguiInputAdapter;

struct FImguiWindowWrapper
{
public:
	static void SetupPlatformInterface(ImGuiPlatformIO* PlatformIO);	
private:
	static void		UE_CreateWindow(ImGuiViewport* viewport);
	static void		UE_DestroyWindow(ImGuiViewport* viewport);
	static void		UE_ShowWindow(ImGuiViewport* viewport);
	static void		UE_UpdateWindow(ImGuiViewport* viewport);
	static ImVec2	UE_GetWindowPos(ImGuiViewport* viewport);
	static void		UE_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos);
	static ImVec2	UE_GetWindowSize(ImGuiViewport* viewport);
	static void		UE_SetWindowSize(ImGuiViewport* viewport, ImVec2 size);
	static void		UE_SetWindowFocus(ImGuiViewport* viewport);
	static bool		UE_GetWindowFocus(ImGuiViewport* viewport);
	static bool		UE_GetWindowMinimized(ImGuiViewport* viewport);
	static void		UE_SetWindowTitle(ImGuiViewport* viewport, const char* title);
	static void		UE_SetWindowAlpha(ImGuiViewport* viewport, float alpha);
	static float	UE_GetWindowDpiScale(ImGuiViewport* viewport);
	static void		UE_OnChangedViewport(ImGuiViewport* viewport);
	static void		UE_SetImeInputPos(ImGuiViewport* viewport, ImVec2 pos);
public:
	static UImguiContext*		CurContext;
	static UImguiInputAdapter*	CurInputAdapter;
};
