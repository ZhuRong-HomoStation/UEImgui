#pragma once
#include "CoreMinimal.h"
#include "ImguiDetailCustomization.generated.h"

UCLASS(Abstract)
class UEIMGUI_API UImguiDetailCustomization : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION()
	virtual UClass* GetSupportClass() { return nullptr; }
	
	UFUNCTION()
	virtual bool SupportSingleObjectEditing() { return false; }

	UFUNCTION()
	virtual bool SupportMultiObjectEditing() { return false; }
	
	UFUNCTION()
	virtual void OnEditSingleObject(UObject* InObject) { }

	UFUNCTION()
	virtual void OnEditMultiObject(TArray<UObject*> InObjects) { }
};

