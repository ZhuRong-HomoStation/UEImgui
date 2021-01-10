#pragma once
#include "CoreMinimal.h"
#include "imgui.h"
#include "ImguiInput/ImguiGlobalInputHook.h"
#include "ImguiInput/ImguiInputAdapterDeferred.h"
#include "UEImgui.generated.h"

/**
 * @brief Built-in imgui texture, like font...
 */
enum class EGlobalImguiTextureId
{
	DefaultFont = -1,
};

/**
 * @brief Imgui resource current is texture, may add material 
 */
USTRUCT()
struct UEIMGUI_API FImguiResource
{
	GENERATED_BODY()
public:
	FImguiResource(const FName& InName = NAME_None, UTexture* SourceObject = nullptr)
		: Name(InName)
		, Source(SourceObject)
	{}

	UPROPERTY()
	FName		Name;
	UPROPERTY()
	UTexture*	Source;
};

DECLARE_DELEGATE_RetVal(bool, FDrawGlobalImgui);

/**
 * @brief Imgui subsystem, interface for a global context of imgui 
 */
UCLASS()
class UUEImgui : public UEngineSubsystem
{
	GENERATED_BODY()
public:
	static UUEImgui& Get() { return *GEngine->GetEngineSubsystem<UUEImgui>(); }

	// Resource action 
	ImTextureID AddResource(FName InResName, UTexture* SourceObj);
	bool IsResourceExist(FName InResName);
	bool IsResourceExist(ImTextureID InID);
	FImguiResource* FindResource(FName InResName);
	FImguiResource* FindResource(ImTextureID InResId);
	void ReleaseResource(FName InResName);
	void ReleaseResource(ImTextureID InID);
	
	// Draw imgui
	void AddGlobalWindow(const FDrawGlobalImgui& InDrawEvent) { AllDrawCallBack.Add(InDrawEvent); }
protected:
	// ~Begin UEngineSubsystem API
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// ~End UEngineSubsystem API
private:
	// Global Context
	void _InitGlobalContext();
	void _ShutDownGlobalContext();
	
	// Default font
	void _InitDefaultFont();
	void _ShutDownDefaultFont();
private:
	// Resources 
	UPROPERTY()
	TMap<int32, FImguiResource>	AllResource;

	// Name find map 
	TMap<FName, int32>			NamedResourceMap;

	// Current unique id 
	int32						CurrentResIdx;
	
	// Global context 
	ImGuiContext*				GlobalContext;

	// Global input adapter 
	UPROPERTY()
	UImguiInputAdapterDeferred*	InputAdapter;

	// Default font atlas 
	ImFontAtlas*				DefaultFont;

	// Draw callbacks 
	TArray<FDrawGlobalImgui>	AllDrawCallBack;
};

