#pragma once
#include "CoreMinimal.h"
#include "imgui.h"
#include <string>
#include "ImguiUEWrap.generated.h"

struct ImDrawData;
class SImguiWidgetRenderProxy;

enum ImguiWindowFlagUE
{
	ImGuiWindowFlags_UEDetail = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar
};

// Unreal draw functions 
namespace ImGui
{
	UEIMGUI_API void StyleColorUE(ImGuiStyle* dst = nullptr);		// Unreal style 
	UEIMGUI_API void StyleColorConfig(ImGuiStyle* dst = nullptr);	// Config style
	UEIMGUI_API bool ShowUEStyleSelector(const char* Label);		// Unreal style editor 
	UEIMGUI_API void ShowUEStyleEditor();							// Unreal style editor 

	UEIMGUI_API void SetCurrentDetail(FName InDetailName);			// internal: used by detail customization 
	UEIMGUI_API void SetCurrentDetailWidget(TWeakPtr<SImguiWidgetRenderProxy> InDetailWidget);	// internal: used by detail customization
	UEIMGUI_API FName GetCurrentDetail();	// Get current detail name 
	UEIMGUI_API void BeginDetail();		// Begin detail for detail customization 
	UEIMGUI_API void EndDetail();		// End detail for detail customization

	UEIMGUI_API void SetCurrentWidget(TWeakPtr<SWidget> InWidget);	// internal: setup widget that render current imgui 
	UEIMGUI_API TWeakPtr<SWidget> GetCurrentWidget();	// internal: get widget that render current imgui

	UEIMGUI_API void Text(const FString& InString);

	UEIMGUI_API void UseInputSystem(const char* label, ImGuiInputTextFlags flags = 0);
	UEIMGUI_API bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
	UEIMGUI_API bool InputTextMultiline(const char* label, std::string* str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
	UEIMGUI_API bool InputTextWithHint(const char* label, const char* hint, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
}

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
	UPROPERTY()
	int		    WindowMenuButtonPosition;   
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
	UPROPERTY()
	int			ColorButtonPosition;        
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
