#pragma once
#include "CoreMinimal.h"
#include "imgui.h"
#include "ImguiUEWrap.generated.h"

struct ImDrawData;
class SImguiWidgetRenderProxy;

enum ImguiWindowFlagUE
{
	ImGuiWindowFlags_UEDetail = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar
};

// Draw tools, for draw imgui in widget 
namespace UEImguiDraw
{
	int32 UEIMGUI_API MakeImgui(
        FSlateWindowElementList& ElementList,
        uint32 InLayer,
        const FSlateRenderTransform& ImguiToRender,
        ImDrawData* DrawData);

	int32 UEIMGUI_API MakeImgui(
		FSlateWindowElementList& ElementList,
		uint32 InLayer,
		const FSlateRenderTransform& ImguiToRender,
		TArray<ImDrawList*>& AllDrawList);

	FORCEINLINE FColor UnpackColor(ImU32 InPackedImguiColor)
	{
		return FColor{
			(uint8)((InPackedImguiColor >> IM_COL32_R_SHIFT) & 0xFF),
			(uint8)((InPackedImguiColor >> IM_COL32_G_SHIFT) & 0xFF),
            (uint8)((InPackedImguiColor >> IM_COL32_B_SHIFT) & 0xFF), 
            (uint8)((InPackedImguiColor >> IM_COL32_A_SHIFT) & 0xFF) };
	}
	
	FORCEINLINE ImU32 PackColor(FColor InColor)
	{
		return	(ImU32)(InColor.R << (24 - IM_COL32_R_SHIFT)) |
				(ImU32)(InColor.G << (24 - IM_COL32_G_SHIFT)) |
				(ImU32)(InColor.B << (24 - IM_COL32_B_SHIFT)) |
				(ImU32)(InColor.A << (24 - IM_COL32_A_SHIFT)) ;
	}
}

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

	UEIMGUI_API void UEText(const FString& InString);
	UEIMGUI_API bool UEInputText(const char* label, FString& InString);
	UEIMGUI_API bool UEInputTextMultiline(const char* label, FString& InString, const FVector2D& InSize);	
}

// Imgui type convert
namespace ImGui
{
#define FORCE_CAST_UE(TUE, TImgui)\
FORCEINLINE const TUE& UEType(const TImgui& InVar) { return *(const TUE*)(&InVar); } \
FORCEINLINE TUE& UEType(TImgui& InVar) { return *(TUE*)(&InVar); } 
#define FORCE_CAST_IM(TUE, TImgui)\
FORCEINLINE const TImgui& ImType(const TUE& InVar) { return *(const TImgui*)(&InVar); } \
FORCEINLINE TImgui& ImType(TUE& InVar) { return *(TImgui*)(&InVar); } 
#define FORCE_CAST_ITEM(TUE, TImgui) \
	FORCE_CAST_UE(TUE, TImgui)\
	FORCE_CAST_IM(TUE, TImgui)

	template<typename TUE,typename TImgui> TUE UEType(TImgui) { static_assert("not support type"); }
	template<typename TUE,typename TImgui> TImgui ImType(TUE) { static_assert("not support type"); }

	FORCE_CAST_ITEM(FLinearColor, ImVec4)
	FORCE_CAST_ITEM(FVector2D, ImVec2)
	FORCE_CAST_IM(FVector4, ImVec4)

	#undef FORCE_CAST_ITEM
#undef FORCE_CAST_UE
#undef FORCE_CAST_IM
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
