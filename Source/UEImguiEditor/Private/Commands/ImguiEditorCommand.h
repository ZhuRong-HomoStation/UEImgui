#pragma once

class FImguiEditorCommand : public TCommands<FImguiEditorCommand>
{
public:
	FImguiEditorCommand()
        : TCommands<FImguiEditorCommand>(
            TEXT("ImguiEditor"),
            NSLOCTEXT("Contexts","ImguiEditor","ImguiEditor Plugin"),
            NAME_None,
            TEXT("ImguiEditorStyle"))
	{}

	virtual void RegisterCommands() override;
	
	TSharedPtr<FUICommandInfo>		OpenImguiDemo;
	TSharedPtr<FUICommandInfo>		OpenStyleEditor;
};
