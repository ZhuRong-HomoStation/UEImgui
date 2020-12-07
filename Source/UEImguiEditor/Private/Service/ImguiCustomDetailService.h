#pragma once
#include "Editor.h"
#include "EditorSubsystem.h"
#include "Services/ImguiDetailCustomization.h"

#include "ImguiCustomDetailService.generated.h"

USTRUCT()
struct FImguiDummy
{
	GENERATED_BODY()
	
};

UCLASS()
class UImguiCustomDetailService : public UEditorSubsystem
{
	GENERATED_BODY()
public:
	static UImguiCustomDetailService& Get() { return *GEditor->GetEditorSubsystem<UImguiCustomDetailService>(); }

	TArray<UImguiDetailCustomization*> GetAllDetailCustomizationOfClass(UClass* InClass);
protected:
	// ~Begin USubsystem API
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// ~End USubsystem API

public:
	UPROPERTY()
	TArray<UClass*>		AllDetailCustomization;
};
