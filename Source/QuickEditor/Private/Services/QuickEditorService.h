#pragma once
#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "QuickEditorService.generated.h"

UCLASS()
class UQuickEditorService : public UEditorSubsystem
{
	GENERATED_BODY()
public:
	UQuickEditorService();

protected:
	// ~Begin USubsystem API 
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// ~End USubsystem API
private:
	void _InitLevelEditMenu();
	void _ShutDownLevelEditMenu();

private:
	TSharedPtr<FUICommandList>		m_MainMenuCmdList;

};
