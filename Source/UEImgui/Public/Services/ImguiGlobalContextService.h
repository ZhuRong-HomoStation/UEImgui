#pragma once
#include "CoreMinimal.h"
#include "Window/IImguiViewport.h"

DECLARE_DELEGATE_RetVal(bool, FDrawGlobalImgui);
class UImguiContext;
class UImguiInputAdapter;

namespace UEImGui
{
	/**
	 * @brief en. Is it time to draw? before you call other global functions, you should check it
	 *		  ch. 是否可以开始绘制，在调用其它函数之前，你需要验证它
	 * 
	 * @param WorldContextObject en. Indicates the world to which the object calling this function belongs, by default, we use GWorld 
	 *							 ch. 指定调用此函数的对象所属的世界，默认情况下使用GWorld
	 *							 
	 * @return en. Is it time to draw, if true, you can call other global functions safe
	 *         ch. 是否可以开始绘制，如果返回True，你可以安全的调用其它全局函数
	 */
	UEIMGUI_API bool TimeToDraw(UObject* WorldContextObject = nullptr);

	/**
	 * @brief en. get global context, which depends on the GameInstance to which the object belongs
	 *		  ch. 得到全局的ImguiContext, 取决于对象所属的GameInstance 
	 * 
	 * @param WorldContextObject en. Indicates the world to which the object calling this function belongs, by default, we use GWorld 
	 *							 ch. 指定调用此函数的对象所属的世界，默认情况下使用GWorld
	 *							 
	 * @return en. current global context
	 *         ch. 当前的ImguiContext
	 */
	UEIMGUI_API UImguiContext* GetGlobalContext(UObject* WorldContextObject = nullptr);
	
	/**
	 * @brief en. get global input adapter, which depends on the GameInstance to which the object belongs 
	 *		  ch. 得到全局的InputAdapter, 取决于对象所属的GameInstance 
	 * 
	 * @param WorldContextObject en. Indicates the world to which the object calling this function belongs, by default, we use GWorld 
	 *							 ch. 指定调用此函数的对象所属的世界，默认情况下使用GWorld
	 * 
	 * @return en. current global context
	 *         ch. 当前的InputAdapter 
	 */
	UEIMGUI_API UImguiInputAdapter* GetGlobalInputAdapter(UObject* WorldContextObject = nullptr);

	/**
	 * @brief en. add and callback for draw imgui windows
	 *        ch. 添加一个用于绘制Imgui窗口的回调
	 * 
	 * @param InGlobalContext en. Draw callback, return should the window draw next frame, if false, it will be removed
	 *                        ch. 回调应返回一个bool用于指定下一帧窗口是否应当被渲染，如果返回false, 回调会被自动移除
	 *
	 * 
	 * @param WorldContextObject en. Indicates the world to which the object calling this function belongs, by default, we use GWorld 
	 *							 ch. 指定调用此函数的对象所属的世界，默认情况下使用GWorld
	 *
	 * @return en. the index of callback in current global context, for remove global window 
	 *         ch. 当前CallBack在回调数组中的位置，可以用来移除callback
	 */
	UEIMGUI_API int32 AddGlobalWindow(const FDrawGlobalImgui& InGlobalContext, UObject* WorldContextObject = nullptr);

	/**
	 * @brief en. remove callback from callback array
	 *        ch. 从回调数组中移除某个回调
	 *        
	 * @param InIndex en. draw callback index, witch returned by AddGlobalWindow
	 *                ch. 绘制回调的位置，由AddGlobalWindow返回
	 * 
	 * @param WorldContextObject en. Indicates the world to which the object calling this function belongs, by default, we use GWorld 
	 *							 ch. 指定调用此函数的对象所属的世界，默认情况下使用GWorld
	 */
	UEIMGUI_API void RemoveGlobalWindow(int32 InIndex, UObject* WorldContextObject = nullptr);

	/**
	 * @brief en. add render proxy, about render proxy, see also IImguiViewport and SImguiRenderProxy 
	 *        ch. 添加一个渲染代理, 关于渲染代理, 参见IImguiViewport和SImguiRenderProxy
	 *
	 * @param InRenderProxy en. the render proxy we want to add 
	 *                      ch. 期望添加的渲染代理
	 *                      
	 * @param WorldContextObject en. Indicates the world to which the object calling this function belongs, by default, we use GWorld 
	 *							 ch. 指定调用此函数的对象所属的世界，默认情况下使用GWorld
	 */
	UEIMGUI_API void AddRenderProxy(TWeakPtr<IImguiViewport> InRenderProxy, UObject* WorldContextObject = nullptr);
	
	/**
	 * @brief en. remove render proxy, about render proxy, see also IImguiViewport and SImguiRenderProxy
	 *        ch. 移除一个渲染代理, 关于渲染代理, 参见IImguiViewport和SImguiRenderProxy
	 * 
	 * @param InRenderProxy en. the render proxy we want to remove 
	 *                      ch. 期望移除的渲染代理
	 * 
	 * @param WorldContextObject en. Indicates the world to which the object calling this function belongs, by default, we use GWorld 
	 *							 ch. 指定调用此函数的对象所属的世界，默认情况下使用GWorld
	 */
	UEIMGUI_API void RemoveRenderProxy(TWeakPtr<IImguiViewport> InRenderProxy, UObject* WorldContextObject = nullptr);
}
