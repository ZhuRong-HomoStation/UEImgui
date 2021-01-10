#include "ImguiWindowManager.h"
#include "imgui.h"
#include "Widgets/SImguiWindow.h"
#include "ImguiInput/ImguiInputAdapter.h"

namespace UEImguiWndManger
{
	TArray<TSharedPtr<SImguiWindow>> ImguiWnds;
	ImGuiContext*		CurContext;
	UImguiInputAdapter*	CurInputAdapter;
}

static void UE_CreateWindow(ImGuiViewport* viewport)
{
	// create a window 
	auto NewWnd = SNew(SImguiWindow)
	.Context(UEImguiWndManger::CurContext)
	.Adapter(UEImguiWndManger::CurInputAdapter)
	.IsToolTip(viewport->Flags & ImGuiViewportFlags_TopMost)
	.IsPopup(viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon);

	// setup window info
	
	
	UEImguiWndManger::ImguiWnds.Add(NewWnd);
}

static void UE_DestroyWindow(ImGuiViewport* viewport)
{
	
}

static void UE_ShowWindow(ImGuiViewport* viewport)
{
	
}

static void UE_UpdateWindow(ImGuiViewport* viewport)
{
	
}

static ImVec2 UE_GetWindowPos(ImGuiViewport* viewport)
{
	return ImVec2();
}

static void UE_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
{
	
}

static ImVec2 UE_GetWindowSize(ImGuiViewport* viewport)
{
	return ImVec2();
}

static void UE_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
{
	
}

static void UE_SetWindowFocus(ImGuiViewport* viewport)
{
	
}

static bool UE_GetWindowFocus(ImGuiViewport* viewport)
{
	return false;
}

static bool UE_GetWindowMinimized(ImGuiViewport* viewport)
{
	return false;
}

static void UE_SetWindowTitle(ImGuiViewport* viewport, const char* title)
{
	
}

static void UE_SetWindowAlpha(ImGuiViewport* viewport, float alpha)
{
	
}

static float UE_GetWindowDpiScale(ImGuiViewport* viewport)
{
	return 0.f;
}

static void UE_OnChangedViewport(ImGuiViewport* viewport)
{
	
}

static void UE_SetImeInputPos(ImGuiViewport* viewport, ImVec2 pos)
{
	
}

void FImguiWindowManager::SetupPlatformInterface(ImGuiPlatformIO* PlatformIO)
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

void FImguiWindowManager::SetCurrentContext(ImGuiContext* InContext)
{
	UEImguiWndManger::CurContext = InContext;
}

void FImguiWindowManager::SetCurrentInputAdapter(UImguiInputAdapter* InInputAdapter)
{
	UEImguiWndManger::CurInputAdapter = InInputAdapter;
}
