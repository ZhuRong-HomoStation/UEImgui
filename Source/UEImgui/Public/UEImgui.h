#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UEIMGUI_API FUEImguiModule : public IModuleInterface
{
public:
	// ~Begin IModuleInterface API 
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// ~End IModuleInterface API 
};
