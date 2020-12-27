#include "Widgets/SImguiWindow.h"
#include "imgui_internal.h"
#include "ImguiWrap/ImguiHelp.h"
#include "ImguiWrap/ImguiUEWrap.h"

void SImguiWindow::Construct(const FArguments& InArgs)
{
	BoundContext = InArgs._Context;
	
	Super::Construct( Super::FArguments()
        .LayoutBorder(FMargin(0))
        .HasCloseButton(false)
		.Type(InArgs._IsMenu ? EWindowType::Menu : InArgs._IsToolTip ? EWindowType::ToolTip : EWindowType::Normal)
		.IsTopmostWindow(InArgs._IsToolTip)
		.IsPopupWindow(InArgs._IsMenu || InArgs._IsToolTip || InArgs._IsPopup)
        .SizingRule(ESizingRule::Autosized)
        .HasCloseButton(false)
        .CreateTitleBar(false)
        .SupportsMaximize(false)
        .SupportsMinimize(false));
}

FReply SImguiWindow::OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent)
{
	UImguiInputAdapter* Adapter = GetAdapter();
	return Adapter ? Adapter->OnKeyChar(InCharacterEvent) : FReply::Unhandled();
}

FReply SImguiWindow::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	UImguiInputAdapter* Adapter = GetAdapter();
	Super::OnKeyDown(MyGeometry, InKeyEvent);
	if (!Adapter) return FReply::Unhandled();
	return Adapter->OnKeyDown(InKeyEvent);
}

FReply SImguiWindow::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	UImguiInputAdapter* Adapter = GetAdapter();
	Super::OnKeyUp(MyGeometry, InKeyEvent);
	if (!Adapter) return FReply::Unhandled();
	return Adapter->OnKeyUp(InKeyEvent);
}

FReply SImguiWindow::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	UImguiInputAdapter* Adapter = GetAdapter();
	if (!Adapter) return FReply::Unhandled();
	return Adapter->OnMouseButtonDown(MouseEvent);
}

FReply SImguiWindow::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	UImguiInputAdapter* Adapter = GetAdapter();
	if (!Adapter) return FReply::Unhandled();
	return Adapter->OnMouseButtonUp(MouseEvent);
}

FReply SImguiWindow::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	UImguiInputAdapter* Adapter = GetAdapter();
	if (!Adapter) return FReply::Unhandled();
	return Adapter->OnMouseButtonDoubleClick(InMouseEvent);
}

FReply SImguiWindow::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	UImguiInputAdapter* Adapter = GetAdapter();
	if (!Adapter) return FReply::Unhandled();
	return Adapter->OnMouseWheel(MouseEvent);
}

FReply SImguiWindow::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	UImguiInputAdapter* Adapter = GetAdapter();
	if (!Adapter) return FReply::Unhandled();
	return Adapter->OnMouseMove(FVector2D::ZeroVector, MouseEvent);
}

bool SImguiWindow::SupportsKeyboardFocus() const
{
	return true;
}

// void SImguiWindow::OnFocusLost(const FFocusEvent& InFocusEvent)
// {
// 	// find window 
// 	UImguiContext* Context = BoundContext.Get();
// 	if (!Context) return;
// 	ImGuiWindow* Wnd = Context->GetContext()->NavWindow;
// 	if (!Wnd) return;
//
// 	// change context and change focus 
// 	ImGuiContext* CurCtx = ImGui::GetCurrentContext();
// 	Context->ApplyContext();
// 	if (WndID.Contains(Wnd->ID))
// 	{
// 		ImGui::FocusWindow(nullptr);
// 	}
// 	ImGui::SetCurrentContext(CurCtx);
// }

// FReply SImguiWindow::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
// {
// 	// search top level window
// 	UImguiContext* Context = BoundContext.Get();
// 	if (!Context) return FReply::Unhandled();
// 	ImGuiWindow* TopWnd = (ImGuiWindow*)Context->GetContext()->WindowsById.GetVoidPtr(TopWndID);
// 	if (!TopWnd) return FReply::Unhandled();
//
// 	ImGuiContext* CurCtx = ImGui::GetCurrentContext();
// 	Context->ApplyContext();
//
// 	ImGui::FocusWindow(TopWnd);
// 	
// 	ImGui::SetCurrentContext(CurCtx);
// 	return FReply::Handled();
// }

FCursorReply SImguiWindow::OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const
{
	UImguiInputAdapter* Adapter = GetAdapter();
	return Adapter ? Adapter->OnCursorQuery(CursorEvent) : FCursorReply::Cursor(EMouseCursor::Default);
}

FVector2D SImguiWindow::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	UImguiContext* UECtx = GetContext();
	if (!UECtx) return FVector2D::ZeroVector;
	ImGuiContext* Ctx = UECtx->GetContext();

	FVector2D OriginPoint = GetCachedGeometry().GetAccumulatedRenderTransform().GetTranslation();
	FVector2D NewDesiredSize(0);

	for (ImGuiID ID : WndID)
	{
		ImGuiWindow* Wnd = (ImGuiWindow*)Ctx->WindowsById.GetVoidPtr(ID);
		NewDesiredSize.X = FMath::Max(Wnd->Size.x, NewDesiredSize.X);
		NewDesiredSize.Y = FMath::Max(Wnd->Size.y, NewDesiredSize.Y);
	}

	return NewDesiredSize;
}

int32 SImguiWindow::OnPaint(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	UImguiContext* UECtx = GetContext();
	if (!UECtx) return LayerId;
	ImGuiContext* Ctx = UECtx->GetContext();

	static TArray<ImDrawList*> AllDrawList;
	AllDrawList.Reset();
	for (ImGuiID ID : WndID)
	{	
		AllDrawList.Add(static_cast<ImGuiWindow*>(Ctx->WindowsById.GetVoidPtr(ID))->DrawList);
	}

	// collect render trans 
	ImGuiWindow* TopWnd = static_cast<ImGuiWindow*>(Ctx->WindowsById.GetVoidPtr(TopWndID));
	if (!TopWnd) return LayerId;
	FSlateRenderTransform ImguiRenderTrans(FVector2D(-TopWnd->Pos.x, -TopWnd->Pos.y));
	
	return UEImguiDraw::MakeImgui(
		OutDrawElements,
		LayerId,
		ImguiRenderTrans,
		AllDrawList);
}
