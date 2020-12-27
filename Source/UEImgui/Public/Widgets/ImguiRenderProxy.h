#pragma once
#include "CoreMinimal.h"
#include "imgui.h"

class FImguiRenderProxy
{
public:
	FImguiRenderProxy() {} 
	virtual ~FImguiRenderProxy() = 0;

	const TArray<ImGuiID>& GetWndIDArr() const { return WndIDArr; }
	TArray<ImGuiID>& GetWndIDArr() { return WndIDArr; }

	ImGuiID GetTopWnd() const { return TopWndID; }
	void SetTopWnd(ImGuiID InID) { TopWndID = InID; }
protected:
	// topmost window 
	ImGuiID				TopWndID;
	// id array for render, include inner child windows, exclude popup child 
	TArray<ImGuiID>		WndIDArr;	
};

