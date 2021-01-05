#include "ImguiWrap/ImguiHelp.h"

#include "imgui_internal.h"
#include "ImguiWrap/ImguiResourceManager.h"

// global array for fast build draw list 
TArray<FSlateVertex> Vertices;
TArray<SlateIndex> Indices;

int32 UEImguiDraw::MakeImgui(
	FSlateWindowElementList& ElementList,
	uint32 InLayer,
	const FSlateRenderTransform& ImguiToRender,
	ImDrawData* DrawData)
{
	if (DrawData->DisplaySize.x <= 0.0f || DrawData->DisplaySize.y <= 0.0f)
	{
		return InLayer + 1;
	}

	for (int i = 0; i < DrawData->CmdListsCount; ++i)
	{
		const ImDrawList* CmdList = DrawData->CmdLists[i];

		// copy vertices
		Vertices.SetNumUninitialized(CmdList->VtxBuffer.size());
		for (int Idx = 0; Idx < CmdList->VtxBuffer.size(); ++Idx)
		{
			const ImDrawVert& ImGuiVertex = CmdList->VtxBuffer[Idx];
			FSlateVertex& SlateVertex = Vertices[Idx];

			SlateVertex.TexCoords[0] = ImGuiVertex.uv.x;
			SlateVertex.TexCoords[1] = ImGuiVertex.uv.y;
			SlateVertex.TexCoords[2] = SlateVertex.TexCoords[3] = 1.f;

			SlateVertex.Position = ImguiToRender.TransformPoint(FVector2D(ImGuiVertex.pos.x,ImGuiVertex.pos.y));
			
			SlateVertex.Color = UnpackColor(ImGuiVertex.col);
		}
		
		for (int CmdIndex = 0; CmdIndex < CmdList->CmdBuffer.Size; ++CmdIndex)
		{
			const ImDrawCmd* Cmd = &CmdList->CmdBuffer[CmdIndex];

			// copy indices
			Indices.SetNumUninitialized(Cmd->ElemCount);
			for (unsigned int Idx = 0; Idx < Cmd->ElemCount; ++Idx)
			{
				Indices[Idx] = CmdList->IdxBuffer[Cmd->IdxOffset + Idx];
			}

			FVector2D Begin(Cmd->ClipRect.x, Cmd->ClipRect.y);
			FVector2D End(Cmd->ClipRect.z, Cmd->ClipRect.w);
			
			Begin = ImguiToRender.TransformPoint(Begin);
			End = ImguiToRender.TransformPoint(End);
			
			ElementList.PushClip(FSlateClippingZone(FSlateRect(Begin, End)));
			auto Handle = UImguiResourceManager::Get().FindHandle(Cmd->TextureId);
			check(Handle.IsValid());
			FSlateDrawElement::MakeCustomVerts(ElementList, InLayer, Handle
                , Vertices, Indices, nullptr, 0, 0);
			ElementList.PopClip();
		}
	}

	return InLayer + 1;
}

int32 UEImguiDraw::MakeImgui(
	FSlateWindowElementList& ElementList,
	uint32 InLayer,
	const FSlateRenderTransform& ImguiToRender,
	TArray<ImDrawList*>& AllDrawList)
{
	if (AllDrawList.Num() == 0) return InLayer + 1;
	
	for (ImDrawList* CmdList : AllDrawList)
	{
		// copy vertices
		Vertices.SetNumUninitialized(CmdList->VtxBuffer.size());
		for (int Idx = 0; Idx < CmdList->VtxBuffer.size(); ++Idx)
		{
			const ImDrawVert& ImGuiVertex = CmdList->VtxBuffer[Idx];
			FSlateVertex& SlateVertex = Vertices[Idx];

			SlateVertex.TexCoords[0] = ImGuiVertex.uv.x;
			SlateVertex.TexCoords[1] = ImGuiVertex.uv.y;
			SlateVertex.TexCoords[2] = SlateVertex.TexCoords[3] = 1.f;

			SlateVertex.Position = ImguiToRender.TransformPoint(FVector2D(ImGuiVertex.pos.x,ImGuiVertex.pos.y));
			
			SlateVertex.Color = UnpackColor(ImGuiVertex.col);
		}
		
		for (int CmdIndex = 0; CmdIndex < CmdList->CmdBuffer.Size; ++CmdIndex)
		{
			const ImDrawCmd* Cmd = &CmdList->CmdBuffer[CmdIndex];
			
			// copy indices
			Indices.SetNumUninitialized(Cmd->ElemCount);
			for (unsigned int Idx = 0; Idx < Cmd->ElemCount; ++Idx)
			{
				Indices[Idx] = CmdList->IdxBuffer[Cmd->IdxOffset + Idx];
			}

			FVector2D Begin(Cmd->ClipRect.x, Cmd->ClipRect.y);
			FVector2D End(Cmd->ClipRect.z, Cmd->ClipRect.w);
			
			Begin = ImguiToRender.TransformPoint(Begin);
			End = ImguiToRender.TransformPoint(End);
			
			ElementList.PushClip(FSlateClippingZone(FSlateRect(Begin, End)));
			auto Handle = UImguiResourceManager::Get().FindHandle(Cmd->TextureId);
			check(Handle.IsValid());
			FSlateDrawElement::MakeCustomVerts(ElementList, InLayer, Handle
                , Vertices, Indices, nullptr, 0, 0);
			ElementList.PopClip();
		}
	}

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
