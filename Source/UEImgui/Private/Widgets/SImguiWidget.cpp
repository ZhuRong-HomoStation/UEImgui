#include "Widgets/SImguiWidget.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Logging.h"
#include "ImguiWrap/ImguiInputAdapter.h"
#include "ImguiWrap/ImguiContext.h"
#include "ImguiWrap/ImguiResourceManager.h"
#include "ImguiWrap/ImguiUEWrap.h"
#include "Render/ImguiDrawer.h"
#include "Widgets/Input/SEditableText.h"

void SImguiWidgetRenderProxy::Construct(const FArguments& InArgs)
{
	HSizingRule = InArgs._HSizingRule;
	VSizingRule = InArgs._VSizingRule;
	bAutoSetWidgetPos = InArgs._AutoSetWidgetPos;
	PersistWndID = InArgs._ProxyWndName.IsEmpty() ? 0 : ImHashStr(TCHAR_TO_UTF8(*InArgs._ProxyWndName));
	Context = InArgs._InContext;
	Adapter = InArgs._InAdapter;
	bBlockInput  = InArgs._BlockInput;
}

void SImguiWidgetRenderProxy::SetContext(UImguiContext* InContext)
{
	check(InContext != nullptr);
	Context = InContext;
	if (Adapter) Adapter->SetContext(Context);
}

void SImguiWidgetRenderProxy::SetAdapter(UImguiInputAdapter* InAdapter)
{
	check(InAdapter != nullptr);
	Adapter = InAdapter;
	if (Context) Adapter->SetContext(Context);	
}

void SImguiWidgetRenderProxy::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(Context);
	Collector.AddReferencedObject(Adapter);
}

FString SImguiWidgetRenderProxy::GetReferencerName() const
{
	return TEXT("ImguiRenderProxy");
}

FReply SImguiWidgetRenderProxy::OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent)
{
	Super::OnKeyChar(MyGeometry, InCharacterEvent);
	if (!Adapter) return FReply::Unhandled();
	FReply AdapterReply = Adapter->OnKeyChar(InCharacterEvent);
	return bBlockInput ? AdapterReply : FReply::Unhandled();
}

FReply SImguiWidgetRenderProxy::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	Super::OnKeyDown(MyGeometry, InKeyEvent);
	if (!Adapter) return FReply::Unhandled();
	FReply AdapterReply = Adapter->OnKeyDown(InKeyEvent);
	return bBlockInput ? AdapterReply : FReply::Unhandled();
}

FReply SImguiWidgetRenderProxy::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	Super::OnKeyUp(MyGeometry, InKeyEvent);
	if (!Adapter) return FReply::Unhandled();
	FReply AdapterReply = Adapter->OnKeyUp(InKeyEvent);
	return bBlockInput ? AdapterReply : FReply::Unhandled();
}

FReply SImguiWidgetRenderProxy::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	Super::OnMouseButtonDown(MyGeometry, MouseEvent);
	if (!Adapter) return FReply::Unhandled();
	FReply AdapterReply = Adapter->OnMouseButtonDown(MouseEvent);
	return bBlockInput ? AdapterReply : FReply::Unhandled();
}

FReply SImguiWidgetRenderProxy::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	Super::OnMouseButtonDown(MyGeometry, MouseEvent);
	if (!Adapter) return FReply::Unhandled();
	FReply AdapterReply = Adapter->OnMouseButtonUp(MouseEvent);
	return bBlockInput ? AdapterReply : FReply::Unhandled();
}

FReply SImguiWidgetRenderProxy::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry,
	const FPointerEvent& InMouseEvent)
{
	Super::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
	if (!Adapter) return FReply::Unhandled();
	FReply AdapterReply = Adapter->OnMouseButtonDoubleClick(InMouseEvent);
	return bBlockInput ? AdapterReply : FReply::Unhandled();
}

FReply SImguiWidgetRenderProxy::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	Super::OnMouseWheel(MyGeometry, MouseEvent);
	if (!Adapter) return FReply::Unhandled();
	FReply AdapterReply = Adapter->OnMouseWheel(MouseEvent);
	return bBlockInput ? AdapterReply : FReply::Unhandled();
}

int32 SImguiWidgetRenderProxy::OnPaint(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	if (!BoundViewport || !BoundViewport->DrawData) return LayerId;

	if (BoundViewport->DrawData->DisplaySize.x <= 0.0f || BoundViewport->DrawData->DisplaySize.y <= 0.0f)
	{
		return LayerId + 1;
	}

	// get vertex offset
	FVector2D ImguiVertexOffset = Args.GetWindowToDesktopTransform() + AllottedGeometry.GetAbsolutePosition() - *(FVector2D*)&BoundViewport->Pos;

	auto Size = OutDrawElements.GetPaintWindow()->GetSizeInScreen();
	auto Drawer = FImguiDrawer::AllocDrawer();
	FMatrix OrthoMatrix(
        FPlane(2.0f / Size.X,   0.0f,			0.0f,			0.0f),
        FPlane(0.0f,			-2.0f / Size.Y,	0.0f,			0.0f),
        FPlane(0.0f,			0.0f,			1.f / 5000.f,	0.0f),
        FPlane(-1,			    1,				0.5f,			1.0f));
	Drawer->SetSlateTransform(ImguiVertexOffset, 1, OrthoMatrix);
	Drawer->SetClipRect(FSlateRect(0,0,Size.X, Size.Y));
	Drawer->SetDrawData(BoundViewport->DrawData);
	FSlateDrawElement::MakeCustom(OutDrawElements, LayerId, Drawer);
	return LayerId + 1;
}

FVector2D SImguiWidgetRenderProxy::ComputeDesiredSize(float) const
{
	UImguiContext* UECtx = GetContext();
	if (!UECtx) return FVector2D::ZeroVector;
	ImGuiContext* Ctx = UECtx->GetContext();

	FVector2D OriginPoint = GetCachedGeometry().GetAccumulatedRenderTransform().GetTranslation();
	FVector2D NewDesiredSize(0);

	ImGuiWindow* Wnd = (ImGuiWindow*)Ctx->WindowsById.GetVoidPtr(PersistWndID);
	// HSizing 
	switch (HSizingRule)
	{
	case EImguiSizingRule::ImSize:
		NewDesiredSize.X = Wnd->Size.x;
		break;
	case EImguiSizingRule::ImContentSize:
		NewDesiredSize.X = Wnd->ContentSize.x + (Wnd->WindowBorderSize + Wnd->WindowPadding.x) * 2;
		break;
	default: ;
	}

	// VSizing
	switch (VSizingRule)
	{
	case EImguiSizingRule::ImSize:
		NewDesiredSize.Y = Wnd->Size.y;
		break;
	case EImguiSizingRule::ImContentSize:
		NewDesiredSize.Y = Wnd->ContentSize.y + (Wnd->WindowBorderSize + Wnd->WindowPadding.y) * 2;
		break;
	default: ;
	}

	return NewDesiredSize;
}

FReply SImguiWidgetRenderProxy::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!GetAdapter()) return FReply::Unhandled();
	FReply AdapterReply = Adapter->OnMouseMove(FVector2D::ZeroVector, MouseEvent);
	return bBlockInput ? AdapterReply : FReply::Unhandled();
}

bool SImguiWidgetRenderProxy::SupportsKeyboardFocus() const
{
	return Adapter && Adapter->CanReceiveKeyboardInput();
}

void SImguiWidgetRenderProxy::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	if (!GetContext()) return;
	// change context 
	ImGuiContext* LastCtx = ImGui::GetCurrentContext();
	ImGuiContext* Ctx = GetContext()->GetContext();
	GetContext()->ApplyContext();
	ImGuiWindow* Wnd = (ImGuiWindow*)Ctx->WindowsById.GetVoidPtr(PersistWndID);
	
	// remove focus
	if (Ctx->ActiveIdWindow && Ctx->ActiveIdWindow->RootWindow != Wnd)
		ImGui::FocusWindow(nullptr);
	
	// resume context 
	ImGui::SetCurrentContext(LastCtx);
}

FCursorReply SImguiWidgetRenderProxy::OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const
{
	Super::OnCursorQuery(MyGeometry, CursorEvent);
	return Adapter->OnCursorQuery(CursorEvent);
}

