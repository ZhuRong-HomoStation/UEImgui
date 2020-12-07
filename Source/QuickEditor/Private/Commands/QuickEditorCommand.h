#pragma once

class FQuickEditorCommand : public TCommands<FQuickEditorCommand>
{
public:
	FQuickEditorCommand()
        : TCommands<FQuickEditorCommand>(
            TEXT("QuickEditor"),
            NSLOCTEXT("Contexts","QuickEditor","QuickEditor Module"),
            NAME_None,
            TEXT("QuickEditorStyle"))
	{}

	virtual void RegisterCommands() override;
};
