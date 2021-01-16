#pragma once
#include "CoreMinimal.h"
#include "imgui.h"
#include <string>

struct ImDrawData;
class SImguiRenderProxy;
class FUEImguiDetail;

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
	UEIMGUI_API void SetCurrentDetailWidget(TWeakPtr<SImguiRenderProxy> InDetailWidget);	// internal: used by detail customization
	UEIMGUI_API FName GetCurrentDetail();		// Get current detail name 
	UEIMGUI_API void BeginDetail();				// Begin detail for detail customization 
	UEIMGUI_API void EndDetail();				// End detail for detail customization

	// =============Text=============
	UEIMGUI_API void Text(const FString& InString);

	// =============InputText=============
	UEIMGUI_API bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
	UEIMGUI_API bool InputTextMultiline(const char* label, std::string* str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
	UEIMGUI_API bool InputTextWithHint(const char* label, const char* hint, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);

	// =============UETypeInput=============
	UEIMGUI_API bool UEColor(const char* InLabel, FColor* InColor, ImGuiColorEditFlags InFlags = 0);
	UEIMGUI_API bool UEColor(const char* InLabel, FLinearColor* InColor, ImGuiColorEditFlags InFlags = 0);

	// =============UEDetail============= 
	UEIMGUI_API bool UEEnum(const char* InLabel ,UEnum* InEnumClass, int64* EnumSource);
	UEIMGUI_API bool UEStruct(UScriptStruct* InStruct, void* InValue);
	UEIMGUI_API bool UEObject(UClass* InClass, void* InValue);
	UEIMGUI_API bool UEProperty(FProperty* InProperty, void* InContainer);
	UEIMGUI_API FUEImguiDetail& GlobalDetailMaker();

}

namespace ImGui
{
	template<typename T>
	FORCEINLINE bool UEEnum(const char* InLabel, T* InEnum)
	{
		int64 Value = (int64)*InEnum;
		bool bHasChanged = UEEnum(InLabel, StaticEnum<T>(), &Value);
		*InEnum = (T)Value;
		return bHasChanged;
	}

	template<typename T>
    FORCEINLINE bool UEStruct(T* InStruct)
	{
		return UEStruct(StaticStruct<T>(), InStruct);
	}

	template <typename T>
	FORCEINLINE bool UEObject(T* InObject)
	{
		return UEObject(StaticClass<T>(), InObject);
	}
}
