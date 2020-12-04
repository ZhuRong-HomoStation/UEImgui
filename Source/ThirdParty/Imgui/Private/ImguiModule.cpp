#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class IMGUI_API FImguiModule : public IModuleInterface
{
public:
	// ~Begin IModuleInterface API 
	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}
	// ~End IModuleInterface API 
};

IMPLEMENT_MODULE(FImguiModule, Imgui)