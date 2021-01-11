#pragma once
#include "CoreMinimal.h"
#include "imgui.h"
#include "ImguiResourceManager.generated.h"

class UImguiContext;
struct ImFontAtlas;

enum class EGlobalImguiTextureId
{
	DefaultFont = -1,
};

USTRUCT()
struct UEIMGUI_API FImguiResource
{
	GENERATED_BODY()
public:
	FImguiResource() = default;
	FImguiResource(const FName& InName, UTexture* SourceObject);
	
	UPROPERTY()
	FName		Name;
	UPROPERTY()
	UTexture*	Source;
};

UCLASS()
class UEIMGUI_API UImguiResourceManager : public UObject
{
	GENERATED_BODY()
public:
	UImguiResourceManager();
	static UImguiResourceManager& Get();
	
	// Resource action
	ImTextureID AddResource(FName InResName, UTexture* SourceObj);
	bool IsResourceExist(FName InResName);
	bool IsResourceExist(ImTextureID InID);
	FImguiResource* FindResource(FName InResName);
	FImguiResource* FindResource(ImTextureID InResId);
	void ReleaseResource(FName InResName);
	void ReleaseResource(ImTextureID InID);

	// Default font
	ImFontAtlas* GetDefaultFont() const { return DefaultFont; }
protected:
	// ~Begin UObject API
	virtual void BeginDestroy() override;
	// ~End UObject API
private:
	// Font
	void _InitDefaultFont();
	void _ShutDownDefaultFont();
private:
	UPROPERTY()
	TMap<int32, FImguiResource>	AllResource;
	TMap<FName, int32>			NamedResourceMap;
	int32						CurrentResIdx;

	ImFontAtlas*				DefaultFont;
};

