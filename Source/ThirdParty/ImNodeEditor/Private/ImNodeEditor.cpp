#include "Modules/ModuleManager.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FImNodeEditor"

class FImNodeEditor : public IModuleInterface
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
	
IMPLEMENT_MODULE(FImNodeEditor, ImNodeEditor);