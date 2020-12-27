#pragma once
#include "CoreMinimal.h"
#include "ImguiWrap/ImguiUEWrap.h"
#include "ImguiConfig.generated.h"

UCLASS(Config=ImGui, DefaultConfig)
class UImguiConfig : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UImguiConfig();
	static UImguiConfig* Get()
	{
		static UImguiConfig* Config = nullptr;
		if (!Config)
		{
			Config = NewObject<UImguiConfig>();
			Config->LoadConfig();
			Config->AddToRoot();
		}
		return Config;
	}

	// set tstyle
	void SetStyle(ImGuiStyle* InStyle);

public:
	// Colors 
	UPROPERTY(Config)
	TArray<FLinearColor>	ImguiColors;

	// Style 
	UPROPERTY(Config)
	FImguiStyle				ImguiStyle;

	// Font 
	UPROPERTY(EditAnywhere, Config)
	FString					FontPath;
};

