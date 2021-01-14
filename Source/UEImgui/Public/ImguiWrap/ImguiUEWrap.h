#pragma once
#include "CoreMinimal.h"
#include "imgui.h"
#include <string>

struct ImDrawData;
class SImguiWidgetRenderProxy;

enum ImguiWindowFlagUE
{
	ImGuiWindowFlags_UEDetail = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking
};

// Unreal draw functions 
namespace ImGui
{
	// =============Unreal Style============= 
	UEIMGUI_API void StyleColorUE(ImGuiStyle* dst = nullptr);		// Unreal style 
	UEIMGUI_API void StyleColorConfig(ImGuiStyle* dst = nullptr);	// Config style
	UEIMGUI_API bool ShowUEStyleSelector(const char* Label);		// Unreal style editor 
	UEIMGUI_API void ShowUEStyleEditor();							// Unreal style editor 

	// =============Detail=============
	UEIMGUI_API void SetCurrentDetail(FName InDetailName);			// internal: used by detail customization 
	UEIMGUI_API void SetCurrentDetailWidget(TWeakPtr<SImguiWidgetRenderProxy> InDetailWidget);	// internal: used by detail customization
	UEIMGUI_API FName GetCurrentDetail();		// Get current detail name 
	UEIMGUI_API void BeginDetail();				// Begin detail for detail customization 
	UEIMGUI_API void EndDetail();				// End detail for detail customization

	// =============Text=============
	UEIMGUI_API void Text(const FString& InString);

	// =============InputText=============
	UEIMGUI_API bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
	UEIMGUI_API bool InputTextMultiline(const char* label, std::string* str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
	UEIMGUI_API bool InputTextWithHint(const char* label, const char* hint, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
}
