#pragma once
#include "CoreMinimal.h"

#include "imgui.h"
#include "Framework/Application/IInputProcessor.h"
#include "Widgets/SImguiWidget.h"

#include "ImguiGlobalContextService.generated.h"

struct ImGuiWindow;
class UImguiContext;
class UImguiInputAdapter;
class UImguiInputAdapterDeferred;
class SImguiWindow;
struct ImDrawData;

DECLARE_DELEGATE_RetVal(bool, FDrawGlobalImgui)

class FImguiGlobalInputHook : public IInputProcessor, public FGCObject
{
public:
	// ~Begin IInputProcessor API 
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override;
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent) override;
	virtual bool HandleMotionDetectedEvent(FSlateApplication& SlateApp, const FMotionEvent& MotionEvent) override;
	// ~End IInputProcessor API

	// ~Begin FGCObject API 
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	// ~End FGCObject API

	void AddAdapter(UImguiInputAdapter* InInputAdapter);
	void RemoveAdapter(UImguiInputAdapter* InInputAdapter);

private:
	TArray<UImguiInputAdapter*>			TargetAdapters;
};

UCLASS()
class UEIMGUI_API UImguiGlobalContextService : public UEngineSubsystem
{
	GENERATED_BODY()
public:
	UImguiGlobalContextService();
	static UImguiGlobalContextService& Get()
	{
		return *GEngine->GetEngineSubsystem<UImguiGlobalContextService>();
	}

	bool TimeToDraw();

	void AddGlobalWindow(const FString& WndName, const FDrawGlobalImgui& InDrawEvent) { AllDrawCallBack.Add(WndName, InDrawEvent); }

	void AddRenderProxy(TWeakPtr<SImguiWidgetRenderProxy> InRenderProxy);
	void RemoveRenderProxy(TWeakPtr<SImguiWidgetRenderProxy> InRenderProxy) { AllRenderProxy.Remove(InRenderProxy); }
	
protected:
	// ~Begin USubsystem API
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// ~End USubsystem API
private:
	void _OnSlatePreTick(float DeltaTime);
	
	void _DrawGlobalImguiWnds();
	TSharedPtr<SImguiWindow> _FindUnrealWindow(ImGuiWindow* InWindow);
	void _DispatchWindows();

	bool _IsParent(ImGuiWindow* InChild, ImGuiWindow* InParent);
	bool _IsClosestParent(ImGuiWindow* InChild, ImGuiWindow* InParent);
	bool _IsToolTip(ImGuiWindow* InWnd);
	bool _IsMenu(ImGuiWindow* InWnd);
	bool _IsPopup(ImGuiWindow* InWnd);
	bool _IsInnerChild(ImGuiWindow* InWnd);
	bool _IsChildOfPopupWnd(ImGuiWindow* InWnd);

	TWeakPtr<SImguiWidgetRenderProxy> _FindRenderProxy(ImGuiWindow* InWindow);
	void _CleanUpRenderProxy();
private:
	UPROPERTY()
	UImguiContext*		GlobalContext;

	UPROPERTY()
	UImguiInputAdapterDeferred*		InputAdapter;

	// render proxy to block render data from dispatch windows 
	TArray<TWeakPtr<SImguiWidgetRenderProxy>>	AllRenderProxy;

	// draw call, for quick create imgui windows 
	TMap<FString ,FDrawGlobalImgui>			AllDrawCallBack;

	// input hook, used to capture mouse input when resizing windows 
	TSharedPtr<FImguiGlobalInputHook>	GlobalInputHook;

	// imgui windows map to unreal windows 
	TMap<ImGuiID, TSharedPtr<SImguiWindow>>	ImguiUnrealWindows;

	// used for unregister delegate 
	FDelegateHandle		PreTickHandle;
};


