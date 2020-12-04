#pragma once
#include "CoreMinimal.h"
#include "imgui.h"
#include "Widgets/SWidget.h"

class UImguiContext;
class UImguiInputAdapter;

DECLARE_DELEGATE(FOnImguiDraw);

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
		, _UseImguiWidgetHeight(true)
		, _UseImguiWidgetWidth(true)
		, _UseImguiWndContentSize(false)
	{}
		SLATE_EVENT(FOnImguiDraw, OnDraw)
		SLATE_ARGUMENT(UImguiContext*, InContext)
		SLATE_ARGUMENT(UImguiInputAdapter*, InAdapter)
		SLATE_ARGUMENT(bool, UseImguiWidgetHeight)
		SLATE_ARGUMENT(bool, UseImguiWidgetWidth)
		// for auto sizing windows 
		SLATE_ARGUMENT(bool, UseImguiWndContentSize)
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
	bool			bUseImguiWidgetHeight;
	bool			bUseImguiWidgetWidth;
	bool			bUseImguiWndContentSize;
	FOnImguiDraw	OnDraw;
};

// imgui draw proxy widget, only do input forward and draw, always used for global context 
class UEIMGUI_API SImguiWidgetRenderProxy : public SImguiWidgetBase
{
	using Super = SImguiWidgetBase;
public:
	SLATE_BEGIN_ARGS(SImguiWidgetRenderProxy)
            : _InContext(nullptr)
            , _InAdapter(nullptr)
            , _UseImguiWidgetHeight(true)
            , _UseImguiWidgetWidth(true)
			, _AutoSetWidgetPos(true)
			, _AutoSetWidgetSize(true)
            , _UseImguiWndContentSize(false)
	{}
	    SLATE_ARGUMENT(UImguiContext*, InContext)
	    SLATE_ARGUMENT(UImguiInputAdapter*, InAdapter)
	    SLATE_ARGUMENT(bool, UseImguiWidgetHeight)
	    SLATE_ARGUMENT(bool, UseImguiWidgetWidth)
	    SLATE_ARGUMENT(bool, AutoSetWidgetPos)
	    SLATE_ARGUMENT(bool, AutoSetWidgetSize)
	    // for auto sizing windows 
	    SLATE_ARGUMENT(bool, UseImguiWndContentSize)
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

	// ~End SWidget API 
private:
	ImGuiID				TopWndID;
	TArray<ImGuiID>		WndID;
	bool			bUseImguiWidgetHeight;
	bool			bUseImguiWidgetWidth;
	bool			bUseImguiWndContentSize;
	bool			bAutoSetWidgetPos;
	bool			bAutoSetWidgetSize;
	FOnImguiDraw	OnDraw;
};
