#pragma once
#include "CoreMinimal.h"
#include "imgui.h"
#include "Services/ImguiGlobalContextService.h"
#include "Widgets/SWidget.h"
#include "Window/IImguiViewport.h"

class UImguiContext;
class UImguiInputAdapter;

DECLARE_DELEGATE(FOnImguiDraw);

enum class EImguiSizingRule
{
	// Desired size is zero, and we won't control imgui wnd size 
	NoSizing ,

	// Desired size is zero, and we use UE Widget size as imgui wnd size 
	UESize ,

	// Desired is imgui wnd size
	ImSize ,

	// Desired is imgui size, and we will use wnd content size
	ImContentSize ,
};

// imgui draw proxy widget, only do input forward and draw, always used for global context 
class UEIMGUI_API SImguiWidgetRenderProxy : public SLeafWidget, public FGCObject, public IImguiViewport
{
	using Super = SLeafWidget;
public:
	SLATE_BEGIN_ARGS(SImguiWidgetRenderProxy)
            : _InContext(nullptr)
			, _InAdapter(nullptr)
			, _HSizingRule(EImguiSizingRule::NoSizing)
			, _VSizingRule(EImguiSizingRule::NoSizing)
			, _AutoSetWidgetPos(true)
			, _BlockInput(true)
	{}
	    SLATE_ARGUMENT(UImguiContext*, InContext)
	    SLATE_ARGUMENT(UImguiInputAdapter*, InAdapter)
		SLATE_ARGUMENT(EImguiSizingRule, HSizingRule)
		SLATE_ARGUMENT(EImguiSizingRule, VSizingRule)
		SLATE_ARGUMENT(const char*, ProxyWndName)
	    SLATE_ARGUMENT(bool, AutoSetWidgetPos)
		SLATE_ARGUMENT(bool, BlockInput)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	UImguiContext* GetContext() const { return Context; }
	UImguiInputAdapter* GetAdapter() const { return Adapter; }
	void SetContext(UImguiContext* InContext);
	void SetAdapter(UImguiInputAdapter* InAdapter);
	
	ImGuiID GetPersistWndID() const { return PersistWndID; }
	void SetPersistWndID(ImGuiID InID) { PersistWndID = InID; }
protected:
	// ~Begin FGCObject API
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;
	// ~End FGCObject API
	
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

	// Cursor
	virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const override;

	// paint and size 
	virtual int32 OnPaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	// ~End SWidget API

	// ~Begin IImguiViewport API
	virtual TSharedPtr<SWindow> GetWindow() override { return CachedWnd.Pin(); }
	virtual void Show(TSharedPtr<SWindow> InParent) override { }
	virtual bool IsPersist() override { return true; }
	virtual ImGuiID GetPersistWindowID() override { return PersistWndID; }
	virtual void Update() override {}
	virtual FVector2D GetPos() override { return GetPaintSpaceGeometry().GetAbsolutePosition(); }
	virtual void SetPos(FVector2D InPos) override {}
	virtual FVector2D GetSize() override { return GetPaintSpaceGeometry().GetAbsoluteSize(); }
	virtual void SetSize(FVector2D InSize) override {}
	virtual bool GetFocus() override { return bHasFocus; }
	virtual void SetFocus() override { FSlateApplication::Get().SetUserFocus(0, AsShared()); }
	virtual bool GetMinimized() override { return CachedWnd.IsValid() ? CachedWnd.Pin()->IsWindowMinimized() : false; }
	virtual void SetTitle(const char* InTitle) override {  }
	virtual void SetAlpha(float InAlpha) override { }
	virtual void SetupViewport(ImGuiViewport* InViewport) override { BoundViewport = InViewport; }
	virtual void SetupInputAdapter(UImguiInputAdapter* ImguiInputAdapter) override { SetAdapter(ImguiInputAdapter); }
	// ~End IImguiViewport API 
protected:
	// cached top side window 
	mutable TWeakPtr<SWindow> CachedWnd;

	// imgui state  
	UImguiContext*		Context;
	UImguiInputAdapter*	Adapter;
	ImGuiViewport*		BoundViewport;

	// proxy setting s 
	ImGuiID				PersistWndID;
	EImguiSizingRule	HSizingRule;
	EImguiSizingRule	VSizingRule;
	bool				bAutoSetWidgetPos;
	bool				bHasFocus;
	bool				bBlockInput;
};

