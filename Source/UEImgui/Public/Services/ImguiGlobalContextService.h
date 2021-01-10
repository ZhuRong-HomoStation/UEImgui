#pragma once
#include "CoreMinimal.h"

#include "imgui.h"
#include "Framework/Application/IInputProcessor.h"
#include "ImguiWrap/ImguiGlobalInputHook.h"
#include "Widgets/SImguiWidget.h"

#include "ImguiGlobalContextService.generated.h"

struct ImGuiWindow;
class UImguiContext;
class UImguiInputAdapter;
class UImguiInputAdapterDeferred;
class SImguiWindow;
struct ImDrawData;

DECLARE_DELEGATE_RetVal(bool, FDrawGlobalImgui)

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
	void RemoveGlobalWindow(const FString& WndName) { AllDrawCallBack.Remove(WndName); }
	
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
	TSharedPtr<SImguiWindow> _FindUnrealWindow(ImGuiWindow* InWindow, bool* IsCreated = nullptr);
	void _DispatchWindows();

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
	TSharedPtr<FImguiGlobalInputHook>		GlobalInputHook;

	// imgui windows map to unreal windows 
	TMap<ImGuiID, TSharedPtr<SImguiWindow>>	ImguiUnrealWindows;

	// used for unregister delegate 
	FDelegateHandle		PreTickHandle;
};


