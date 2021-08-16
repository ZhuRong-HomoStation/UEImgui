#pragma once
#include "CoreMinimal.h"
#include "imgui.h"
#include "ImguiWrap/ImguiContext.h"
#include "ImguiWrap/ImguiInputAdapter.h"
#include "Widgets/SWindow.h"
#include "Window/IImguiViewport.h"

class UEIMGUI_API SImguiWindow : public SWindow, public IImguiViewport
{
	using Super = SWindow;
public:
	SLATE_BEGIN_ARGS(SImguiWindow)
		: _Context(nullptr)
		, _Viewport(nullptr)
		, _IsToolTip(false)
	{}
		SLATE_ARGUMENT(TWeakObjectPtr<UImguiContext>, Context)
		SLATE_ARGUMENT(TWeakObjectPtr<UImguiInputAdapter>, Adapter)
		SLATE_ARGUMENT(ImGuiViewport*, Viewport)
		SLATE_ARGUMENT(bool, IsToolTip)	
		SLATE_ARGUMENT(bool, IsPopup)	
		SLATE_ARGUMENT(bool, TakeFocusWhenShow)	
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	// Context action	
	UImguiContext* GetContext() const { return BoundContext.Get(); }
	void SetContext(UImguiContext* InContext) { BoundContext = InContext; }

	UImguiInputAdapter* GetAdapter() const { return BoundAdapter.Get(); }
	void SetAdapter(UImguiInputAdapter* InAdapter) { BoundAdapter = InAdapter; }
protected:
	// ~Begin SWidget API 
	
	// receive key input 
	virtual FReply OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	// receive mouse input
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	
	// Focus
	virtual bool SupportsKeyboardFocus() const override;
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;

	// cursor
	virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const override;

	// draw 
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;
	virtual int32 OnPaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;
	// ~Begin SWidget API

	// ~Begin IImguiViewport API 
	virtual void SetupContext(UImguiContext* InCtx) override { SetContext(InCtx); }
	virtual TSharedPtr<SWindow> GetWindow() override { return StaticCastSharedRef<SWindow>(AsShared()); }
	virtual void Show(TSharedPtr<SWindow> InParent) override;
	virtual void Update() override;
	virtual FVector2D GetPos() override;
	virtual void SetPos(FVector2D InPos) override;
	virtual FVector2D GetSize() override;
	virtual void SetSize(FVector2D InSize) override;
	virtual bool GetFocus() override;
	virtual void SetFocus() override;
	virtual bool GetMinimized() override;
	virtual void SetTitle(const char* InTitle) override;
	virtual void SetAlpha(float InAlpha) override;
	virtual void SetupViewport(ImGuiViewport* InViewport) override;
	virtual void SetupInputAdapter(UImguiInputAdapter* ImguiInputAdapter) override;
	virtual float GetDpiScale() override;
	// ~End IImguiViewport API 
private:
	ImGuiViewport*						BoundViewport;
	TWeakObjectPtr<UImguiContext>		BoundContext;
	TWeakObjectPtr<UImguiInputAdapter>	BoundAdapter;
	bool								bInFocus = false;
};
