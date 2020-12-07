#include "Widgets/SImguiWidget.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Logging.h"
#include "ImguiWrap/ImguiInputAdapter.h"
#include "ImguiWrap/ImguiContext.h"
#include "ImguiWrap/ImguiResourceManager.h"
#include "ImguiWrap/ImguiUEWrap.h"
#include "Widgets/Input/SEditableText.h"

void SImguiWidgetBase::Construct(const FArguments& InArgs)
{
	Context = InArgs._InContext;
	Adapter = InArgs._InAdapter;

	// validate 
	if (!Context)
	{
		Context = UImguiResourceManager::Get().CreateContext();
	}
	if (!Adapter)
	{
		Adapter = NewObject<UImguiInputAdapter>();
	}
	Adapter->SetContext(Context);
}

SImguiWidgetBase::~SImguiWidgetBase()
{
	UImguiResourceManager::Get().ReleaseContext(Context);
}

void SImguiWidgetBase::SetContext(UImguiContext* InContext)
{
	check(Context != nullptr);
	Context = InContext;
	Adapter->SetContext(Context);
}

void SImguiWidgetBase::SetAdapter(UImguiInputAdapter* InAdapter)
{
	check(InAdapter != nullptr);
	Adapter = InAdapter;
	Adapter->SetContext(Context);	
}

void SImguiWidgetBase::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(Context);
	Collector.AddReferencedObject(Adapter);
}

FString SImguiWidgetBase::GetReferencerName() const
{
	return TEXT("ImguiWidget");
}

FReply SImguiWidgetBase::OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent)
{
	Super::OnKeyChar(MyGeometry, InCharacterEvent);
	if (!Adapter) return FReply::Unhandled();
	return Adapter->OnKeyChar(InCharacterEvent);
}

FReply SImguiWidgetBase::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	Super::OnKeyDown(MyGeometry, InKeyEvent);
	if (!Adapter) return FReply::Unhandled();
	return Adapter->OnKeyDown(InKeyEvent);
}

FReply SImguiWidgetBase::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	Super::OnKeyUp(MyGeometry, InKeyEvent);
	if (!Adapter) return FReply::Unhandled();
	return Adapter->OnKeyUp(InKeyEvent);
}

FReply SImguiWidgetBase::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	Super::OnMouseButtonDown(MyGeometry, MouseEvent);
	if (!Adapter) return FReply::Unhandled();
	return Adapter->OnMouseButtonDown(MouseEvent);
}

FReply SImguiWidgetBase::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	Super::OnMouseButtonDown(MyGeometry, MouseEvent);
	if (!Adapter) return FReply::Unhandled();
	return Adapter->OnMouseButtonUp(MouseEvent);}

FReply SImguiWidgetBase::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	Super::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
	if (!Adapter) return FReply::Unhandled();
	return Adapter->OnMouseButtonDoubleClick(InMouseEvent);
}

FReply SImguiWidgetBase::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	Super::OnMouseWheel(MyGeometry, MouseEvent);
	if (!Adapter) return FReply::Unhandled();
	return Adapter->OnMouseWheel(MouseEvent);
}

FReply SImguiWidgetBase::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	Super::OnMouseMove(MyGeometry, MouseEvent);
	if (!Adapter) return FReply::Unhandled();
	return Adapter->OnMouseMove(this, MyGeometry, MouseEvent);
}

bool SImguiWidgetBase::SupportsKeyboardFocus() const
{
	return Adapter && Adapter->CanReceiveKeyboardInput();
}

FCursorReply SImguiWidgetBase::OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const
{
	Super::OnCursorQuery(MyGeometry, CursorEvent);
	return Adapter->OnCursorQuery(CursorEvent);
}

void SImguiWidget::Construct(const FArguments& InArgs)
{
	bUseImguiWidgetWidth = InArgs._UseImguiWidgetWidth;
	bUseImguiWidgetHeight = InArgs._UseImguiWidgetHeight;
	bUseImguiWndContentSize = InArgs._UseImguiWndContentSize;
	OnDraw = InArgs._OnDraw;
	
	// construct parent 
	Super::Construct(
        Super::FArguments()
        .InContext(InArgs._InContext)
        .InAdapter(InArgs._InAdapter));
}

int32 SImguiWidget::OnPaint(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	// set display size 
	GetContext()->GetIO()->DisplaySize = { AllottedGeometry.GetAbsoluteSize().X, AllottedGeometry.GetAbsoluteSize().Y };
	GetContext()->GetIO()->DeltaTime = FSlateApplication::Get().GetDeltaTime();
	
	// set context and begin a new frame
	auto OldContext = ImGui::GetCurrentContext();
	GetContext()->ApplyContext();
	ImGui::NewFrame();

	// draw
	OnDraw.ExecuteIfBound();
	
	// render
	ImGui::Render();

	// draw to widget 
	ImDrawData* DrawData = ImGui::GetDrawData();

	// revert context 
	ImGui::SetCurrentContext(OldContext);

	return UEImguiDraw::MakeImgui(
		OutDrawElements,
		LayerId,
		AllottedGeometry.GetAccumulatedRenderTransform(),
		DrawData);
}

FVector2D SImguiWidget::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	ImGuiContext* Ctx = GetContext()->GetContext();

	FVector2D Size(0);
	for (int i = 0; i < Ctx->Windows.size(); ++i)
	{
		ImGuiWindow* Wnd = Ctx->Windows[i];
		if (Wnd && Wnd->WasActive)
		{
			if (bUseImguiWidgetWidth) Size.X = FMath::Max(Wnd->Pos.x +
				bUseImguiWndContentSize ?
				Wnd->ContentSize.x + (Wnd->WindowBorderSize + Wnd->WindowPadding.x) * 2 : Wnd->Size.x, Size.X);
			if (bUseImguiWidgetHeight) Size.Y = FMath::Max(Wnd->Pos.y +
				bUseImguiWndContentSize ?
				Wnd->ContentSize.y + (Wnd->WindowBorderSize + Wnd->WindowPadding.y) * 2 : Wnd->Size.y, Size.Y);
		}
	}
	
	return Size;
}

void SImguiWidgetRenderProxy::Construct(const FArguments& InArgs)
{
	bUseImguiWidgetWidth = InArgs._UseImguiWidgetWidth;
	bUseImguiWidgetHeight = InArgs._UseImguiWidgetHeight;
	bUseImguiWndContentSize = InArgs._UseImguiWndContentSize;
	bAutoSetWidgetPos = InArgs._AutoSetWidgetPos;
	bAutoSetWidgetSize = InArgs._AutoSetWidgetSize;
	
	// construct parent 
	Super::Construct(
        Super::FArguments()
        .InContext(InArgs._InContext)
        .InAdapter(InArgs._InAdapter));
}

FReply SImguiWidgetRenderProxy::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	Super::OnMouseMove(MyGeometry, MouseEvent);
	if (!GetAdapter()) return FReply::Unhandled();
	return GetAdapter()->OnMouseMove(FVector2D::ZeroVector, MouseEvent);
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
	UImguiContext* UECtx = GetContext();
	if (!UECtx) return LayerId;
	ImGuiContext* Ctx = UECtx->GetContext();
	ImGuiWindow* Wnd = static_cast<ImGuiWindow*>(Ctx->WindowsById.GetVoidPtr(TopWndID));
	if (!Wnd) return LayerId;
	
	FSlateRenderTransform AccumulatedTran = GetCachedGeometry().GetAccumulatedRenderTransform();
	
	if (bAutoSetWidgetPos)
	{
		Wnd->Pos = { AccumulatedTran.GetTranslation().X, AccumulatedTran.GetTranslation().Y };
	}

	if (bAutoSetWidgetSize)
	{
		if (!bUseImguiWidgetHeight)
		{
			Wnd->Size.y = AllottedGeometry.GetAbsoluteSize().Y;
		}
		if (!bUseImguiWidgetWidth)
		{
			Wnd->Size.x = AllottedGeometry.GetAbsoluteSize().X;			
		}
	}
	
	static TArray<ImDrawList*> AllDrawList;
	AllDrawList.Reset();
	for (ImGuiID ID : WndID)
	{	
		AllDrawList.Add(static_cast<ImGuiWindow*>(Ctx->WindowsById.GetVoidPtr(ID))->DrawList);
	}
	
	return UEImguiDraw::MakeImgui(
        OutDrawElements,
        LayerId,
        AccumulatedTran.Inverse().Concatenate(AllottedGeometry.GetAccumulatedRenderTransform()),
        AllDrawList);
}

FVector2D SImguiWidgetRenderProxy::ComputeDesiredSize(float) const
{
	UImguiContext* UECtx = GetContext();
	if (!UECtx) return FVector2D::ZeroVector;
	ImGuiContext* Ctx = UECtx->GetContext();

	FVector2D OriginPoint = GetCachedGeometry().GetAccumulatedRenderTransform().GetTranslation();
	FVector2D NewDesiredSize(0);

	for (ImGuiID ID : WndID)
	{
		ImGuiWindow* Wnd = (ImGuiWindow*)Ctx->WindowsById.GetVoidPtr(ID);
		if (bUseImguiWidgetWidth) NewDesiredSize.X = FMath::Max(Wnd->Pos.x +
                bUseImguiWndContentSize ?
                Wnd->ContentSize.x + (Wnd->WindowBorderSize + Wnd->WindowPadding.x) * 2 : Wnd->Size.x, NewDesiredSize.X);
		if (bUseImguiWidgetHeight) NewDesiredSize.Y = FMath::Max(Wnd->Pos.y +
                bUseImguiWndContentSize ?
                Wnd->ContentSize.y + (Wnd->WindowBorderSize + Wnd->WindowPadding.y) * 2 : Wnd->Size.y, NewDesiredSize.Y);
	}

	return NewDesiredSize;
}
