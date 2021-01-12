#pragma once
#include "CoreMinimal.h"
#include "Window/IImguiViewport.h"

DECLARE_DELEGATE_RetVal(bool, FDrawGlobalImgui);

namespace UEImGui
{
	UEIMGUI_API bool	TimeToDraw(UObject* WorldContextObject = nullptr);
	UEIMGUI_API int32	AddGlobalWindow(const FDrawGlobalImgui& InGlobalContext, UObject* WorldContextObject = nullptr);
	UEIMGUI_API void	RemoveGlobalWindow(int32 InIndex, UObject* WorldContextObject = nullptr);
	UEIMGUI_API void	AddRenderProxy(TWeakPtr<IImguiViewport> InRenderProxy, UObject* WorldContextObject = nullptr);
	UEIMGUI_API void	RemoveRenderProxy(TWeakPtr<IImguiViewport> InRenderProxy, UObject* WorldContextObject = nullptr);
}