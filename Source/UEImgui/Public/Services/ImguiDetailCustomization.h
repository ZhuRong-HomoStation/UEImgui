#pragma once
#include "CoreMinimal.h"
#include "ImguiDetailCustomization.generated.h"

/**
 * @brief en. for imgui detail customization, inherit to customize your detail
 *        ch. 用于使用Imgui扩展Detail, 继承这个类即可使用 
 */
UCLASS(Abstract)
class UEIMGUI_API UImguiDetailCustomization : public UObject
{
	GENERATED_BODY()
public:
	
	/**
	 * @brief en. return class that we wan't to customize
	 *        ch. 返回我们希望定制的类 
	 * 
	 * @return en. class that we wan't to customize
	 *         ch. 我们期望定制的类 
	 */
	UFUNCTION()
	virtual UClass* GetSupportClass() { return nullptr; }

	/**
	 * @brief en. return does we support the class
	 *        ch. 返回我们是否支持某个类的定制
	 * 
	 * @return en. does we support customize of the class
	 *         ch. 我们是否支持某个类的定制
	 */
	UFUNCTION()
	virtual bool IsSupportClass(UClass* InClass) { return false; }
	
	/**
	 * @brief en. return does we support customize when editing single object 
	 *        ch. 返回是否支持单物体编辑时候的定制
	 *        
	 * @return en. does we support customize when editing single object
	 *         ch. 是否支持单物体编辑时候的定制
	 */
	UFUNCTION()
	virtual bool SupportSingleObjectEditing() { return false; }

	/**
	* @brief en. return does we support customize when editing multi object 
	*        ch. 返回是否支持多物体编辑时候的定制
	*        
	* @return en. does we support customize when editing multi object
	*         ch. 是否支持多物体编辑时候的定制
	*/
	UFUNCTION()
	virtual bool SupportMultiObjectEditing() { return false; }

	/**
	 * @brief en. customize object's detail when editing single object, we can also use member function OnGui() in object 
	 *        ch. 自定义单物体编辑时的detail, 我们还可以使用 OnGUI() 成员函数来实现这个功能 
	 * 
	 * @param InObject en. the object we are editing
	 *                 ch. 我们当前正在编辑的物体 
	 */
	UFUNCTION()
	virtual void OnEditSingleObject(UObject* InObject) { }

	/**
	 * @brief en. customize object's detail when editing multi object 
	 *        ch. 自定义多物体编辑时的detail 
	 * 
	 * @param InObjects en. the object we are editing
	 *                 ch. 我们当前正在编辑的物体 
	 */
	UFUNCTION()
	virtual void OnEditMultiObject(TArray<UObject*> InObjects) { }
};

