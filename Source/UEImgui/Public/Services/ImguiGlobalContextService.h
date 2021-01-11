#pragma once
#include "CoreMinimal.h"
#include "imgui.h"
#include "Framework/Application/IInputProcessor.h"
#include "ImguiWrap/ImguiGlobalInputHook.h"
#include "Widgets/SImguiWidget.h"
#include "ImguiWrap/ImguiContext.h"
#include "ImguiGlobalContextService.generated.h"

struct ImGuiWindow;
class UImguiContext;
class UImguiInputAdapter;
class UImguiInputAdapterDeferred;
class SImguiWindow;
struct ImDrawData;

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

	UImguiContext* GetGlobalContext() { return GlobalContext; }
protected:
	// ~Begin USubsystem API
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// ~End USubsystem API
private:
	void _OnSlatePreTick(float DeltaTime);
private:
	UPROPERTY()
	UImguiContext*					GlobalContext;

	UPROPERTY()
	UImguiInputAdapterDeferred*		InputAdapter;

	// input hook, used to capture mouse input when resizing windows 
	TSharedPtr<FImguiGlobalInputHook>		GlobalInputHook;

	// imgui windows map to unreal windows 
	TMap<ImGuiID, TSharedPtr<SImguiWindow>>	ImguiUnrealWindows;

	// used for unregister delegate 
	FDelegateHandle		PreTickHandle;
};


