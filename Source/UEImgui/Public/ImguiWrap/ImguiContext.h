#pragma once
#include "CoreMinimal.h"
#include "imgui.h"
#include "Widgets/SImguiWidget.h"
#include "Window/IImguiViewport.h"
#include "ImguiContext.generated.h"

class UImguiInputAdapter;

DECLARE_DELEGATE_RetVal(bool, FDrawGlobalImgui);

/*
 * hold an imgui environment 
 */
UCLASS(CustomConstructor)
class UEIMGUI_API UImguiContext : public UObject
{
	GENERATED_BODY()
	friend struct FImguiWindowWrapper;
public:
	// init and shutdown
	void Init(TSharedPtr<IImguiViewport> InMainViewPort, ImFontAtlas* InDefaultFontAtlas = nullptr);
	bool IsInit() const { return Context != nullptr; }
	void UpdateSize();
	void ShutDown();

	// context config
	void EnableDocking(bool bInEnable);
	bool EnableDocking();
	void EnableViewport(bool bInEnable);
	bool EnableViewport();
	void EnableDPIScale(bool bInEnable);
	bool EnableDPIScale();
	void EnableNoAutoMergeViewport(bool bInIsEnable);
	bool EnableNoAutoMergeViewport();
	
	// global draw
	int32 AddGlobalWindow(const FDrawGlobalImgui& InCallBack) { return AllDrawCallBack.Add(InCallBack); }
	void RemoveGlobalWindow(int32 InIndex) { AllDrawCallBack[InIndex].Unbind(); AllDrawCallBack.RemoveAt(InIndex);}

	// proxy
	void AddRenderProxy(TWeakPtr<IImguiViewport> InRenderProxy);
	void RemoveRenderProxy(TWeakPtr<IImguiViewport> InRenderProxy);
	
	// context operation 
	ImGuiContext* GetContext() const { return Context; }
	ImGuiIO* GetIO() const;
	ImGuiStyle* GetStyle() const;
	UImguiInputAdapter* GetDefaultInputAdapter() const { return DefaultAdapter; }
	void SetDefaultInputAdapter(UImguiInputAdapter* InInputAdapter) { DefaultAdapter = InInputAdapter; }

	// life time function
	void ApplyContext();
	void NewFrame(float DeltaTime);
	void DrawGlobal();
	void Render();
	void UpdateViewport(UImguiInputAdapter* InAdapter);
protected:
	// ~Begin UObject API
	virtual void BeginDestroy() override;
	// ~End UObject API 
private:
	// window platform 
	void	_CreateWindow(ImGuiViewport* viewport, UImguiInputAdapter* InInputAdapter);
	void	_DestroyWindow(ImGuiViewport* viewport);
	void	_ShowWindow(ImGuiViewport* viewport);
	void	_UpdateWindow(ImGuiViewport* viewport);
	ImVec2	_GetWindowPos(ImGuiViewport* viewport);
	void	_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos);
	ImVec2	_GetWindowSize(ImGuiViewport* viewport);
	void	_SetWindowSize(ImGuiViewport* viewport, ImVec2 size);
	void	_SetWindowFocus(ImGuiViewport* viewport);
	bool	_GetWindowFocus(ImGuiViewport* viewport);
	bool	_GetWindowMinimized(ImGuiViewport* viewport);
	void	_SetWindowTitle(ImGuiViewport* viewport, const char* title);
	void	_SetWindowAlpha(ImGuiViewport* viewport, float alpha);
	float	_GetWindowDpiScale(ImGuiViewport* viewport);
	void	_OnChangedViewport(ImGuiViewport* viewport);
	void	_SetImeInputPos(ImGuiViewport* viewport, ImVec2 pos);

	// setup  
	void _SetupImguiContext();
	void _SetUpDefaultFont();

	// help function 
	TSharedPtr<IImguiViewport> _SafeFindViewport(ImGuiViewport* InViewport, bool bNeedShow = true);
	TWeakPtr<IImguiViewport> _DispatchProxy(ImGuiViewport* InViewport, UImguiInputAdapter* InInputAdapter);
private:
	// main viewport of this context 
	TSharedPtr<IImguiViewport>			MainViewPort;

	// all draw callback   
	TSparseArray<FDrawGlobalImgui>			AllDrawCallBack;

	// render proxy, steal render data form window dispatch 
	TArray<TWeakPtr<IImguiViewport>>	AllRenderProxy;

	// dispatched window, here keep reference to prevent GC 
	TArray<TSharedPtr<IImguiViewport>>	AllDispatchedViewport;

	// imgui context 
	ImGuiContext*						Context;

	// default input adapter
	UImguiInputAdapter*					DefaultAdapter;

	// imgui viewport map to UE viewport, for fast and safe search 
	TMap<ImGuiViewport*, TWeakPtr<IImguiViewport>>	ImViewportToUE;
};
