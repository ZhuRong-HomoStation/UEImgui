#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

struct ImPlotContext;

class FUEImguiEditor : public IModuleInterface
{
public:
	// ~Begin IModuleInterface API 
	void StartupModule() override;
	void ShutdownModule() override;
	// ~End IModuleInterface API

private:
	void _InitDetailExtension();
	void _ShutDownDetailExtension();
	
	void _InitMenu();
	void _ShutDownMenu();

	void _ExtendMenu(FMenuBuilder& InBuilder);
};
