#include "ImguiWrap/ImguiHelp.h"

#include "imgui_internal.h"
#include "ImguiWrap/ImguiResourceManager.h"
#include "Render/ImguiDrawer.h"

// global array for fast build draw list 
TArray<FSlateVertex> ImguiVertices;
TArray<SlateIndex> ImguiIndices;

int32 UEImguiDraw::MakeImgui(
	FSlateWindowElementList& ElementList,
	uint32 InLayer,
	const FSlateRenderTransform& ImguiToRender,
    FSlateRect ClipRect,
	ImDrawData* DrawData)
{
	if (DrawData->DisplaySize.x <= 0.0f || DrawData->DisplaySize.y <= 0.0f)
	{
		return InLayer + 1;
	}

	auto Size = ElementList.GetPaintWindow()->GetSizeInScreen();
	auto Drawer = FImguiDrawer::AllocDrawer();
	FMatrix OrthoMatrix(
		FPlane(2.0f / Size.X,   0.0f,			0.0f,			0.0f),
        FPlane(0.0f,			-2.0f / Size.Y,	0.0f,			0.0f),
        FPlane(0.0f,			0.0f,			1.f / 5000.f,	0.0f),
        FPlane(-1,			    1,				0.5f,			1.0f));
	Drawer->SetSlateTransform(ImguiToRender.GetTranslation(), 1, OrthoMatrix);
	Drawer->SetClipRect(FSlateRect(0,0,Size.X, Size.Y));
	Drawer->SetDrawData(DrawData);
	FSlateDrawElement::MakeCustom(ElementList, InLayer, Drawer);
	return InLayer + 1;
}

int32 UEImguiDraw::MakeImgui(
	FSlateWindowElementList& ElementList,
	uint32 InLayer,
	const FSlateRenderTransform& ImguiToRender,
    FSlateRect ClipRect,
	TArray<ImDrawList*>& AllDrawList)
{
	if (AllDrawList.Num() == 0) return InLayer + 1;

	auto Size = ElementList.GetPaintWindow()->GetSizeInScreen();
	auto Drawer = FImguiDrawer::AllocDrawer();
	FMatrix OrthoMatrix(
        FPlane(2.0f / Size.X,0.0f,			0.0f,		0.0f),
        FPlane(0.0f,			-2.0f / Size.Y,	0.0f,		0.0f),
        FPlane(0.0f,			0.0f,			1.f / 5000.f,0.0f),
        FPlane(-1,			1,				0.5f,		1.0f));
	Drawer->SetSlateTransform(ImguiToRender.GetTranslation(), 1, OrthoMatrix);
	Drawer->SetClipRect(FSlateRect(0,0,Size.X, Size.Y));
	Drawer->SetDrawData(AllDrawList);
	FSlateDrawElement::MakeCustom(ElementList, InLayer, Drawer);
	return InLayer + 1;
}

bool UEImguiHelp::IsParent(ImGuiWindow* InChild, ImGuiWindow* InParent)
{
	while (InChild->ParentWindow)
	{
		if (InChild->ParentWindow == InParent) return true;
		InChild = InChild->ParentWindow;
	}
	return false;
}

bool UEImguiHelp::IsClosestParent(ImGuiWindow* InChild, ImGuiWindow* InParent)
{
	while (InChild->ParentWindow)
	{
		if (InChild->ParentWindow == InParent) return true;
		if (IsPopup(InChild->ParentWindow) || IsMenu(InChild->ParentWindow) || IsToolTip(InChild->ParentWindow)) return false;
		InChild = InChild->ParentWindow;
	}
	return false;
}

bool UEImguiHelp::IsToolTip(ImGuiWindow* InWnd)
{
	return FCStringAnsi::Strncmp(InWnd->Name, "##Tooltip", 9) == 0;
}

bool UEImguiHelp::IsMenu(ImGuiWindow* InWnd)
{
	return FCStringAnsi::Strncmp(InWnd->Name, "##Menu", 6) == 0;
}

bool UEImguiHelp::IsPopup(ImGuiWindow* InWnd)
{
	return
        FCStringAnsi::Strncmp(InWnd->Name, "##Popup", 7) == 0 ||
        FCStringAnsi::Strncmp(InWnd->Name, "##Combo", 7) == 0;
}

bool UEImguiHelp::IsInnerChild(ImGuiWindow* InWnd)
{
	char* Pos = FCStringAnsi::Strchr(InWnd->Name, '/');
	if (!Pos) return false;
	Pos = FCStringAnsi::Strchr(Pos, '_');
	if (!Pos) return false;
	++Pos;
	while (*Pos)
	{
		if (!(*Pos >= '0' && *Pos <= '9') && !(*Pos >= 'A' && *Pos <= 'F')) return false;
		++Pos;
	}
	return true;
}

bool UEImguiHelp::IsChildOfPopupWnd(ImGuiWindow* InWnd)
{
	return InWnd->ParentWindow ?
        IsMenu(InWnd->ParentWindow) || IsToolTip(InWnd->ParentWindow) || IsPopup(InWnd->ParentWindow)
        : false;
}
