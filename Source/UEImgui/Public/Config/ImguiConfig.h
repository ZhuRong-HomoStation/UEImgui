#pragma once
#include "CoreMinimal.h"
#include "ImguiWrap/ImguiUEWrap.h"
#include "Engine/DeveloperSettings.h"
#include "ImguiConfig.generated.h"

// Imgui setting wrap 
USTRUCT()
struct FImguiStyle
{
	GENERATED_BODY()
public:
	UPROPERTY()
	float       Alpha;        
	UPROPERTY()              
	FVector2f   WindowPadding;         
	UPROPERTY()     
	float       WindowRounding;         
	UPROPERTY()    
	float       WindowBorderSize;           
	UPROPERTY()
	FVector2f   WindowMinSize;              
	UPROPERTY()
	FVector2f   WindowTitleAlign;           
	// UPROPERTY()
	ImGuiDir    WindowMenuButtonPosition = ImGuiDir_Left;   
	UPROPERTY()
	float       ChildRounding;              
	UPROPERTY()
	float       ChildBorderSize;            
	UPROPERTY()
	float       PopupRounding;              
	UPROPERTY()
	float       PopupBorderSize;            
	UPROPERTY()
	FVector2f   FramePadding;               
	UPROPERTY()
	float       FrameRounding;              
	UPROPERTY()
	float       FrameBorderSize;            
	UPROPERTY()
	FVector2f   ItemSpacing;                
	UPROPERTY()
	FVector2f   ItemInnerSpacing;           
	UPROPERTY()
	FVector2f   CellPadding;                
	UPROPERTY()
	FVector2f   TouchExtraPadding;          
	UPROPERTY()
	float       IndentSpacing;              
	UPROPERTY()
	float       ColumnsMinSpacing;          
	UPROPERTY()
	float       ScrollbarSize;              
	UPROPERTY()
	float       ScrollbarRounding;          
	UPROPERTY()
	float       GrabMinSize;                
	UPROPERTY()
	float       GrabRounding;               
	UPROPERTY()
	float       LogSliderDeadzone;          
	UPROPERTY()
	float       TabRounding;                
	UPROPERTY()
	float       TabBorderSize;              
	UPROPERTY()
	float       TabMinWidthForCloseButton;  
	// UPROPERTY()
	ImGuiDir    ColorButtonPosition = ImGuiDir_Right;        
	UPROPERTY()
	FVector2f   ButtonTextAlign;            
	UPROPERTY()
	FVector2f   SelectableTextAlign;        
	UPROPERTY()
	FVector2f   DisplayWindowPadding;       
	UPROPERTY()
	FVector2f   DisplaySafeAreaPadding;     
	UPROPERTY()
	float       MouseCursorScale;           
	UPROPERTY()
	bool        AntiAliasedLines;           
	UPROPERTY()
	bool        AntiAliasedLinesUseTex;     
	UPROPERTY()
	bool        AntiAliasedFill;            
	UPROPERTY()
	float       CurveTessellationTol;       
	UPROPERTY()
	float       CircleSegmentMaxError;      
};

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

	// set style
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

	// Save layout
	UPROPERTY(EditAnywhere, Config)
	bool					bSaveLayout = true;
};