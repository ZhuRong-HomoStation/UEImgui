#pragma once
#include "CoreMinimal.h"

struct IImguiViewport
{
	~IImguiViewport() {}

	virtual TSharedPtr<SWindow> GetWindow() = 0;
	virtual ImGuiID GetPersistWindowID() { return 0; }
	virtual bool IsPersist() { return false; }
	virtual void Show(TSharedPtr<SWindow> InParent) = 0;
	virtual void Update() = 0;
	virtual FVector2D GetPos() = 0;
	virtual void SetPos(FVector2D InPos) = 0;
	virtual FVector2D GetSize() = 0;
	virtual void SetSize(FVector2D InSize) = 0;
	virtual bool GetFocus() = 0;
	virtual void SetFocus() = 0;
	virtual bool GetMinimized() = 0;
	virtual void SetTitle(const char* InTitle) = 0;
	virtual void SetAlpha(float InAlpha) = 0;
	virtual float GetDpiScale() { return 0.f; }
	virtual void OnChangeViewport() {}
	virtual void SetupViewport(ImGuiViewport* InViewport) = 0;
	virtual void SetupInputAdapter(UImguiInputAdapter* ImguiInputAdapter) = 0;
};