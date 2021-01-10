#pragma once
#include "CoreMinimal.h"
#include "imgui.h"
#include "ImguiContext.generated.h"

class UImguiInputAdapter;
/*
 * hold an imgui environment 
 */
UCLASS(CustomConstructor)
class UEIMGUI_API UImguiContext : public UObject
{
	GENERATED_BODY()
	friend struct FImguiWindowWrapper;
protected:
	// ~Begin UObject API
	virtual void BeginDestroy() override;
	// ~End UObject API 
public:
	UImguiContext(const FObjectInitializer& InInitializer);

	void Init(ImFontAtlas* InDefaultFontAtlas = nullptr);
	void Reset();
	void ShutDown();
	
	// context operation 
	void ApplyContext();
	ImGuiContext* GetContext() const { return Context; }
	ImGuiIO* GetIO() const;
	ImGuiStyle* GetStyle() const;
private:
	static void		_CreateWindow(ImGuiViewport* viewport, UImguiInputAdapter* InInputAdapter);
	static void		_DestroyWindow(ImGuiViewport* viewport);
	static void		_ShowWindow(ImGuiViewport* viewport);
	static void		_UpdateWindow(ImGuiViewport* viewport);
	static ImVec2	_GetWindowPos(ImGuiViewport* viewport);
	static void		_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos);
	static ImVec2	_GetWindowSize(ImGuiViewport* viewport);
	static void		_SetWindowSize(ImGuiViewport* viewport, ImVec2 size);
	static void		_SetWindowFocus(ImGuiViewport* viewport);
	static bool		_GetWindowFocus(ImGuiViewport* viewport);
	static bool		_GetWindowMinimized(ImGuiViewport* viewport);
	static void		_SetWindowTitle(ImGuiViewport* viewport, const char* title);
	static void		_SetWindowAlpha(ImGuiViewport* viewport, float alpha);
	static float	_GetWindowDpiScale(ImGuiViewport* viewport);
	static void		_OnChangedViewport(ImGuiViewport* viewport);
	static void		_SetImeInputPos(ImGuiViewport* viewport, ImVec2 pos);
private:
	void _SetupImguiContext();
	void _SetUpDefaultFont();
private:
	ImGuiContext*		Context;
};
