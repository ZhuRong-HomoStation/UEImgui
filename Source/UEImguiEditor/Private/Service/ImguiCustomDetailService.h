#pragma once
#include "EditorSubsystem.h"
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
	// ~Begin USubsystem API
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// ~End USubsystem API 
};
