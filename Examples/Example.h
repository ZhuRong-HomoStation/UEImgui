#pragma once
#include "CoreMinimal.h"

#include "Services/ImguiDetailCustomization.h"

#include "Example.generated.h"

UCLASS()
class AImguiExampleActor : public AActor
{
	GENERATED_BODY()
public:
	AImguiExampleActor();

	// open imgui demo window(use global context)
	void OpenImguiDemoGlobal();
	
	// open imgui demo dock window(use global context)
	void OpenImguiDemoDockGlobal();
	
	// open imgui demo window(use stand alone context)
	void OpenImguiDemoStandAlone();
	
	// use OnGUI function for customize detail panel 
	UFUNCTION()
	void OnGUI();

protected:
	UPROPERTY(EditAnywhere)
	USceneComponent* Root;
};

UCLASS()
class UImguiDetailCustomizationExample : public UImguiDetailCustomization
{
	GENERATED_BODY()
public:
	// target class 
	virtual UClass* GetSupportClass() override { return AActor::StaticClass(); }

	// support single object editing 
	virtual bool SupportSingleObjectEditing() override { return true; }
	virtual void OnEditSingleObject(UObject* InObject) override;

	// support mult object editing 
	virtual bool SupportMultiObjectEditing() override { return true; }
	virtual void OnEditMultiObject(TArray<UObject*> InObjects) override;
};


