#pragma once
#include "CoreMinimal.h"
#include "imgui.h"
#include "Services/ImguiGlobalContextService.h"
#include "Widgets/SWidget.h"
#include "Window/IImguiViewport.h"

class UImguiContext;
class UImguiInputAdapter;

DECLARE_DELEGATE(FOnImguiDraw);

/**
 * @brief Imgui sizing rule, control the policy between UE widget size and Imgui widget size 
 */
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

/**
 * @brief en. Render proxy can steal render data that assigned by ProxyWndName or PersistWndID
 *        ch. Render Proxy 可以从渲染数据中"偷"出对应窗口的Viewport来自己渲染，这个窗口由初始化传入的ProxyWndName计算的PersistWndID决定
 */
class UEIMGUI_API SImguiRenderProxy : public SLeafWidget, public FGCObject, public IImguiViewport
{
	using Super = SLeafWidget;
public:
	SLATE_BEGIN_ARGS(SImguiRenderProxy)
            : _InContext(nullptr)
			, _InAdapter(nullptr)
			, _HSizingRule(EImguiSizingRule::NoSizing)
			, _VSizingRule(EImguiSizingRule::NoSizing)
			, _ProxyWndName(nullptr)
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

	// proxy settings 
	ImGuiID				PersistWndID;
	EImguiSizingRule	HSizingRule;
	EImguiSizingRule	VSizingRule;
	bool				bAutoSetWidgetPos;
	bool				bHasFocus;
	bool				bBlockInput;
};

/**
 * @brief en. imgui widget, host a imgui context, and draw self as main viewport
 *        ch. Imgui控件，主持一个imgui的Context，并且将自己作为主视口绘制 
 */
class UEIMGUI_API SImguiWidget : public SLeafWidget, public FGCObject, public IImguiViewport
{
	using Super = SLeafWidget;
public:
	SLATE_BEGIN_ARGS(SImguiWidget)
            : _BlockInput(true)
	{}
	    SLATE_ARGUMENT(bool, BlockInput)
	SLATE_END_ARGS()
};

