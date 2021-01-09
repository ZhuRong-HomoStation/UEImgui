#include "Interfaces/IPluginManager.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FUEImguiModule"

class UEIMGUI_API FUEImguiModule : public IModuleInterface
{
	public:
	// ~Begin IModuleInterface API 
	virtual void StartupModule() override
	{
		// register shader dictionary 
		FString ShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("UEImgui"))->GetBaseDir(),TEXT("Shaders"));
		AddShaderSourceDirectoryMapping(TEXT("/Plugin/UEImgui"), ShaderDir);
	}
	virtual void ShutdownModule() override
	{
		
	}
	// ~End IModuleInterface API 
};

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUEImguiModule, UEImgui)