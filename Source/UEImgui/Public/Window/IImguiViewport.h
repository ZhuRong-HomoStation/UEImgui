#pragma once
#include "CoreMinimal.h"
#include "imgui.h"

class UImguiInputAdapter;
class UImguiContext;

/**
 * @brief Imgui viewport you can inherit it to customize your render proxy 
 */
struct IImguiViewport
{
	virtual ~IImguiViewport() {}

	// setup context
	virtual void SetupContext(UImguiContext* InCtx) = 0;
	
	// get the window that the viewport belong to 
	virtual TSharedPtr<SWindow> GetWindow() = 0;

	// get persist window ID, which indicate proxy window 
	virtual ImGuiID GetPersistWindowID() { return 0; }

	// is this window a render proxy? 
	virtual bool IsPersist() { return false; }

	// show window, Input is the parent window of viewport 
	virtual void Show(TSharedPtr<SWindow> InParent) = 0;

	// update viewport 
	virtual void Update() = 0;

	// get viewport position in screen 
	virtual FVector2D GetPos() = 0;

	// set viewport position in screen 
	virtual void SetPos(FVector2D InPos) = 0;

	// get viewport absolute size 
	virtual FVector2D GetSize() = 0;

	// set viewport absolute size 
	virtual void SetSize(FVector2D InSize) = 0;

	// is viewport has focus? 
	virtual bool GetFocus() = 0;

	// set viewport focus, may be you should bring window to front 
	virtual void SetFocus() = 0;

	// is viewport window minimized 
	virtual bool GetMinimized() = 0;

	// set viewport window title 
	virtual void SetTitle(const char* InTitle) = 0;

	// set viewport alpha 
	virtual void SetAlpha(float InAlpha) = 0;

	// get viewport DPI Scale, in Unreal, we always return 1
	virtual float GetDpiScale() { return 1.f; }

	// notify viewport changed, general, we needn't process it 
	virtual void OnChangeViewport() {}

	// setup Imgui viewport that we bound  
	virtual void SetupViewport(ImGuiViewport* InViewport) = 0;

	// setup Input adapter for message reroute 
	virtual void SetupInputAdapter(UImguiInputAdapter* ImguiInputAdapter) = 0;
};
