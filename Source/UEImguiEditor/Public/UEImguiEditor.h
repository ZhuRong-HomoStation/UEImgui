#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

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
	
	void _InitCommand();
	void _ShutDownCommand();
	
	void _InitMenu();
	void _ShutDownMenu();
private:
	TSharedPtr<FUICommandList>	m_CmdList;
};
