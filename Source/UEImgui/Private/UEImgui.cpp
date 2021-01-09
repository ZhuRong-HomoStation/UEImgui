#include "UEImgui.h"
#include "Interfaces/IPluginManager.h"
#define LOCTEXT_NAMESPACE "FUEImguiModule"

void FUEImguiModule::StartupModule()
{
	// register shader dictionary 
	FString ShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("UEImgui"))->GetBaseDir(),TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/UEImgui"), ShaderDir);
}

void FUEImguiModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUEImguiModule, UEImgui)