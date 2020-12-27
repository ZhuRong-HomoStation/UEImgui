#pragma once
#include "CoreMinimal.h"
#include "imgui.h"

class SImguiWidgetRenderProxy;
struct ImGuiWindow;

// Draw tools, for draw imgui in widget 
namespace UEImguiDraw
{
	int32 UEIMGUI_API MakeImgui(
        FSlateWindowElementList& ElementList,
        uint32 InLayer,
        const FSlateRenderTransform& ImguiToRender,
        ImDrawData* DrawData);

	int32 UEIMGUI_API MakeImgui(
        FSlateWindowElementList& ElementList,
        uint32 InLayer,
        const FSlateRenderTransform& ImguiToRender,
        TArray<ImDrawList*>& AllDrawList);

	FORCEINLINE FColor UnpackColor(ImU32 InPackedImguiColor)
	{
		return FColor{
			(uint8)((InPackedImguiColor >> IM_COL32_R_SHIFT) & 0xFF),
            (uint8)((InPackedImguiColor >> IM_COL32_G_SHIFT) & 0xFF),
            (uint8)((InPackedImguiColor >> IM_COL32_B_SHIFT) & 0xFF), 
            (uint8)((InPackedImguiColor >> IM_COL32_A_SHIFT) & 0xFF) };
	}
	
	FORCEINLINE ImU32 PackColor(FColor InColor)
	{
		return	(ImU32)(InColor.R << (24 - IM_COL32_R_SHIFT)) |
                (ImU32)(InColor.G << (24 - IM_COL32_G_SHIFT)) |
                (ImU32)(InColor.B << (24 - IM_COL32_B_SHIFT)) |
                (ImU32)(InColor.A << (24 - IM_COL32_A_SHIFT)) ;
	}
}

// Imgui Helps
namespace UEImguiHelp
{
	bool IsParent(ImGuiWindow* InChild, ImGuiWindow* InParent);
    bool IsClosestParent(ImGuiWindow* InChild, ImGuiWindow* InParent);
    bool IsToolTip(ImGuiWindow* InWnd);
    bool IsMenu(ImGuiWindow* InWnd);
    bool IsPopup(ImGuiWindow* InWnd);
    bool IsInnerChild(ImGuiWindow* InWnd);
    bool IsChildOfPopupWnd(ImGuiWindow* InWnd);
}


