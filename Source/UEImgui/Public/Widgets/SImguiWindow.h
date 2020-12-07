#pragma once
#include "CoreMinimal.h"
#include "imgui.h"
#include "ImguiWrap/ImguiContext.h"
#include "ImguiWrap/ImguiInputAdapter.h"
#include "Widgets/SWindow.h"

class UEIMGUI_API SImguiWindow : public SWindow
{
	using Super = SWindow;
public:
	SLATE_BEGIN_ARGS(SImguiWindow)
		: _Context(nullptr)
		, _IsMenu(false)
		, _IsToolTip(false)
	{}
		SLATE_ARGUMENT(TWeakObjectPtr<UImguiContext>, Context)
		SLATE_ARGUMENT(TWeakObjectPtr<UImguiInputAdapter>, Adapter)
		SLATE_ARGUMENT(bool, IsMenu)
		SLATE_ARGUMENT(bool, IsToolTip)	
		SLATE_ARGUMENT(bool, IsPopup)	
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	// Context action
	const TArray<ImGuiID>& GetWndID() const { return WndID; }
	TArray<ImGuiID>& GetWndID() { return WndID; }

	ImGuiID GetTopWnd() const { return TopWndID; }
	void SetTopWnd(ImGuiID InID) { TopWndID = InID; }
	
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
	// virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;
	// virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;

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
private:
	ImGuiID								TopWndID;
	TArray<ImGuiID>						WndID;
	TWeakObjectPtr<UImguiContext>		BoundContext;
	TWeakObjectPtr<UImguiInputAdapter>	BoundAdapter;
};
