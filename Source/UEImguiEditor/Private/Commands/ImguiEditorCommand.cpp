#include "ImguiEditorCommand.h"

#define LOCTEXT_NAMESPACE "FImguiEditorModule"

void FImguiEditorCommand::RegisterCommands()
{
	UI_COMMAND(OpenImguiDemo, "OpenImguiDemo", "Bring up ImguiDemo window", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(OpenStyleEditor, "OpenStyleEditor", "Bring up Style Editor window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
