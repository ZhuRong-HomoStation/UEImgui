#include "QuickEditor.h"
#include "Logging.h"
#include "Commands/QuickEditorCommand.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FQuickEditor"

void FQuickEditor::StartupModule()
{
}

void FQuickEditor::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FQuickEditor, QuickEditor);