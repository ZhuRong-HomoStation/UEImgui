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

// base class, implement input forward 
class UEIMGUI_API SImguiWidgetBase : public SLeafWidget, public FGCObject
{
	using Super = SWidget;
public:
	SLATE_BEGIN_ARGS(SImguiWidgetBase)
		: _InContext(nullptr)
		, _InAdapter(nullptr)
	{}
		SLATE_ARGUMENT(UImguiContext*, InContext)
		SLATE_ARGUMENT(UImguiInputAdapter*, InAdapter)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	~SImguiWidgetBase();

	UImguiContext* GetContext() const { return Context; }
	UImguiInputAdapter* GetAdapter() const { return Adapter; }
	void SetContext(UImguiContext* InContext);
	void SetAdapter(UImguiInputAdapter* InAdapter);
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

	// cursor
	virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const override;
	// ~End SWidget API 
protected:
	bool					bHasFocus = false;
private:
	UImguiContext*			Context;
	UImguiInputAdapter*		Adapter;
};

// standard imgui widget, always used for standard context 
class UEIMGUI_API SImguiWidget : public SImguiWidgetBase
{
	using Super = SImguiWidgetBase;
public:
	SLATE_BEGIN_ARGS(SImguiWidget)
        : _InContext(nullptr)
        , _InAdapter(nullptr)
		, _HSizingRule(EImguiSizingRule::NoSizing)
		, _VSizingRule(EImguiSizingRule::NoSizing)
	{}
		SLATE_EVENT(FOnImguiDraw, OnDraw)
		SLATE_ARGUMENT(UImguiContext*, InContext)
		SLATE_ARGUMENT(UImguiInputAdapter*, InAdapter)
		SLATE_ARGUMENT(EImguiSizingRule, HSizingRule)
		SLATE_ARGUMENT(EImguiSizingRule, VSizingRule)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
protected:
	// ~Begin SWidget API
	virtual int32 OnPaint(
        const FPaintArgs& Args,
        const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect, 
        FSlateWindowElementList& OutDrawElements,
        int32 LayerId,
        const FWidgetStyle& InWidgetStyle,
        bool bParentEnabled) const override;
	
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;
	// ~End SWidget API
private:
	EImguiSizingRule	HSizingRule;
	EImguiSizingRule	VSizingRule;	
	FOnImguiDraw		OnDraw;
};

// imgui draw proxy widget, only do input forward and draw, always used for global context 
class UEIMGUI_API SImguiWidgetRenderProxy : public SImguiWidgetBase, public IImguiViewport
{
	using Super = SImguiWidgetBase;
public:
	SLATE_BEGIN_ARGS(SImguiWidgetRenderProxy)
            : _InContext(nullptr)
			, _InAdapter(nullptr)
			, _HSizingRule(EImguiSizingRule::NoSizing)
			, _VSizingRule(EImguiSizingRule::NoSizing)
			, _AutoSetWidgetPos(true)
	{}
	    SLATE_ARGUMENT(UImguiContext*, InContext)
	    SLATE_ARGUMENT(UImguiInputAdapter*, InAdapter)
		SLATE_ARGUMENT(EImguiSizingRule, HSizingRule)
		SLATE_ARGUMENT(EImguiSizingRule, VSizingRule)
		SLATE_ARGUMENT(FString, ProxyWndName)
	    SLATE_ARGUMENT(bool, AutoSetWidgetPos)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	const TArray<ImGuiID>& GetWndID() const { return WndID; }
	TArray<ImGuiID>& GetWndID() { return WndID; }

	ImGuiID GetTopWnd() const { return TopWndID; }
	void SetTopWnd(ImGuiID InID) { TopWndID = InID; }
protected:
	// ~Begin SWidget API
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual int32 OnPaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;
	// ~End SWidget API

	// ~Begin IImguiViewport API
	virtual TSharedPtr<SWindow> GetWindow() override { return CachedWnd; }
	virtual void Show(TSharedPtr<SWindow> InParent) override { }
	virtual void Update() override {}
	virtual FVector2D GetPos() override { return GetTickSpaceGeometry().GetAbsolutePosition(); }
	virtual void SetPos(FVector2D InPos) override {  }
	virtual FVector2D GetSize() override { return GetTickSpaceGeometry().GetAbsoluteSize(); }
	virtual void SetSize(FVector2D InSize) override {}
	virtual bool GetFocus() override { return bHasFocus; }
	virtual void SetFocus() override { FSlateApplication::Get().SetUserFocus(0, AsShared()); }
	virtual bool GetMinimized() override { return CachedWnd.IsValid() ? CachedWnd->IsWindowMinimized() : false; }
	virtual void SetTitle(const char* InTitle) override { CachedWnd->SetTitle(FText::FromString(FString(InTitle))); }
	virtual void SetAlpha(float InAlpha) override { }
	virtual void SetupViewport(ImGuiViewport* InViewport) override {  }
	virtual void SetupInputAdapter(UImguiInputAdapter* ImguiInputAdapter) override { SetAdapter(ImguiInputAdapter); }
	// ~End IImguiViewport API 
protected:
	mutable TSharedPtr<SWindow> CachedWnd;
	ImGuiID				TopWndID;
	TArray<ImGuiID>		WndID;
	EImguiSizingRule	HSizingRule;
	EImguiSizingRule	VSizingRule;
	bool				bAutoSetWidgetPos;
};

// global imgui widget
class UEIMGUI_API SGlobalImguiWidget : public SImguiWidgetRenderProxy
{
	using Super = SImguiWidgetRenderProxy;
public:
	SLATE_BEGIN_ARGS(SGlobalImguiWidget)
            : _InContext(nullptr)
            , _InAdapter(nullptr)
            , _HSizingRule(EImguiSizingRule::NoSizing)
            , _VSizingRule(EImguiSizingRule::NoSizing)
            , _AutoSetWidgetPos(true)
	{}
		SLATE_ARGUMENT(UImguiContext*, InContext)
	    SLATE_ARGUMENT(UImguiInputAdapter*, InAdapter)
	    SLATE_ARGUMENT(EImguiSizingRule, HSizingRule)
	    SLATE_ARGUMENT(EImguiSizingRule, VSizingRule)
	    SLATE_ARGUMENT(bool, AutoSetWidgetPos)
		SLATE_ARGUMENT(FString, WndName)
		SLATE_EVENT(FOnImguiDraw, OnDraw)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	~SGlobalImguiWidget();
	
private:
	int32				WindowId;
	FOnImguiDraw		DrawCallBack;
};
