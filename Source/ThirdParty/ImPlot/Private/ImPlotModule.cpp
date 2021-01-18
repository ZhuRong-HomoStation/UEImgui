#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FImPlot"

class FImPlot : public IModuleInterface
{
public:
	void StartupModule() override
	{
		
	}
	void ShutdownModule() override
	{
		
	}
};

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FImPlot, ImPlot);