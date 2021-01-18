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
	FVector2D   WindowPadding;         
	UPROPERTY()     
	float       WindowRounding;         
	UPROPERTY()    
	float       WindowBorderSize;           
	UPROPERTY()
	FVector2D   WindowMinSize;              
	UPROPERTY()
	FVector2D   WindowTitleAlign;           
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
	FVector2D   FramePadding;               
	UPROPERTY()
	float       FrameRounding;              
	UPROPERTY()
	float       FrameBorderSize;            
	UPROPERTY()
	FVector2D   ItemSpacing;                
	UPROPERTY()
	FVector2D   ItemInnerSpacing;           
	UPROPERTY()
	FVector2D   CellPadding;                
	UPROPERTY()
	FVector2D   TouchExtraPadding;          
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
	FVector2D   ButtonTextAlign;            
	UPROPERTY()
	FVector2D   SelectableTextAlign;        
	UPROPERTY()
	FVector2D   DisplayWindowPadding;       
	UPROPERTY()
	FVector2D   DisplaySafeAreaPadding;     
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
};