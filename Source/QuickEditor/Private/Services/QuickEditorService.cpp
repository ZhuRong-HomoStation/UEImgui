#include "QuickEditorService.h"

#include "LevelEditor.h"
#include "Logging.h"
#include "ToolMenus.h"
#include "Commands/QuickEditorCommand.h"

UQuickEditorService::UQuickEditorService()
{
}

void UQuickEditorService::Initialize(FSubsystemCollectionBase& Collection)
{
	_InitLevelEditMenu();
}

void UQuickEditorService::Deinitialize()
{
	_ShutDownLevelEditMenu();
}

void UQuickEditorService::_InitLevelEditMenu()
{
	// init command 
	FQuickEditorCommand::Register();

	// collect all editor menu commands
	TArray<UClass*> AllClass;
	GetDerivedClasses(UObject::StaticClass(), AllClass, true);

	// search functions
	TMap<FString, UFunction*> AllMenuFuncs;
	for (UClass* Class : AllClass)
	{
		for (TFieldIterator<UFunction> It(Class); It; ++It)
		{
			if (!(It->FunctionFlags & EFunctionFlags::FUNC_Static)) continue;
			if (!It->HasMetaData(TEXT("QEMenuItem"))) continue;
			FString Path = It->GetMetaData(TEXT("QEMenuItem"));

			// validate path 
			check(FPaths::ValidatePath(Path));

			// validate function type
			check(It->NumParms == 0);
			check(It->ReturnValueOffset == 65535);
			
			AllMenuFuncs.Add(Path, *It);
		}
	}

	// build menu tree
	AllMenuFuncs.KeySort(TLess<>());
	struct FMenuNode
	{
		FString			MenuName;
		TArray<FMenuNode> ChildMenus;
		UFunction*		BoundFunction = nullptr;
	};
	TArray<FMenuNode> NodeEntries;
	for (auto & MenuCommand : AllMenuFuncs)
	{
		FString Path = MenuCommand.Key;
		FString Section;

		// find root 
		Path.Split(TEXT("/"), &Section, &Path);
		FMenuNode* CurNode = NodeEntries.FindByPredicate([&](const FMenuNode& InNode){ return InNode.MenuName == Section; });
		if (!CurNode)
		{
			NodeEntries.Emplace(FMenuNode{Section});
			CurNode = &NodeEntries.Top();
		}

		// build path nodes 
		while (Path.Split(TEXT("/"), &Section, &Path))
		{
			FMenuNode* FoundNode = CurNode->ChildMenus.FindByPredicate([&](const FMenuNode& InNode){ return InNode.MenuName == Section; });
			if (FoundNode)
			{
				// node conflict 
				check(FoundNode->BoundFunction == nullptr);
				CurNode = FoundNode;
			}
			else
			{
				// add child node
				CurNode->ChildMenus.Emplace(FMenuNode{Section});
				CurNode = &CurNode->ChildMenus.Top();
			}
		}

		// add menu entry
		check(!Path.IsEmpty());
		FMenuNode* FoundNode = CurNode->ChildMenus.FindByPredicate([&](const FMenuNode& InNode){ return InNode.MenuName == Path; });
		check(FoundNode == nullptr);	// node conflict 
		CurNode->ChildMenus.Emplace(FMenuNode{Path, {}, MenuCommand.Value});
	}

	// map commands
	m_MainMenuCmdList = MakeShareable(new FUICommandList);
	// TMap<FString, TSharedPtr<FUICommandInfo>> AllCommand;
	// for (auto& Func : AllMenuFuncs)
	// {
	// 	FString EndName;
	// 	Func.Key.Split("/", nullptr, &EndName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	// 	TSharedPtr<FUICommandInfo> Cmd;
	// 	MakeUICommand_InternalUseOnly(const_cast<FQuickEditorCommand*>(&FQuickEditorCommand::Get()),
	// 		Cmd,
	// 		TEXT("QuickEditor"),
	// 		*Func.Key,
	// 		*(Func.Key + TEXT("_ToolTip")),
	// 		TCHAR_TO_UTF8(*(TEXT(".") + Func.Key)),
	// 		*EndName,
	// 		TEXT(""),
	// 		EUserInterfaceActionType::Button,
	// 		FInputGesture());
	//
	// 	m_MainMenuCmdList->MapAction(Cmd, FExecuteAction::CreateLambda([]
	// 	{
	// 		FFrame
	// 	}));
	// }
	
	// add extensions
	struct FBuildTools
	{
		static void _ExtendMenu(FMenuBuilder& InBuilder, const FMenuNode& InNode)
		{
			if (InNode.BoundFunction)
			{
				InBuilder.AddMenuEntry(FText::FromString(InNode.MenuName), FText::GetEmpty(), FSlateIcon(),
                    FUIAction(FExecuteAction::CreateLambda([=]
                    {
                        FFrame Stack(nullptr, InNode.BoundFunction, nullptr);
                        InNode.BoundFunction->Invoke(nullptr, Stack, nullptr);
                    })));
			}
			else
			{
				InBuilder.AddSubMenu(FText::FromString(InNode.MenuName), FText::GetEmpty(),
                FNewMenuDelegate::CreateLambda([&](FMenuBuilder& InBuilder, const FMenuNode& InNode)
                {
                    for (auto& ChildMenu : InNode.ChildMenus)
                    {
                        _ExtendMenu(InBuilder, ChildMenu);
                    }
                }, InNode));
			}
		}
	};

	auto ExtendMenuBar = [&](FMenuBarBuilder& InBuilder, FMenuNode Node)
	{
		InBuilder.AddPullDownMenu(
            FText::FromString(Node.MenuName),
            FText::GetEmpty(),
            FNewMenuDelegate::CreateLambda([](FMenuBuilder& InBuilder, const FMenuNode& InNode)
            {
	            FBuildTools::_ExtendMenu(InBuilder, InNode);
            }, Node));
	};
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	for (FMenuNode& Entry : NodeEntries)
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuBarExtension(
            "Help",
            EExtensionHook::After,
            m_MainMenuCmdList,
            FMenuBarExtensionDelegate::CreateLambda(ExtendMenuBar, Entry));
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
}

void UQuickEditorService::_ShutDownLevelEditMenu()
{
	// shutdown command 
	FQuickEditorCommand::Unregister();
	m_MainMenuCmdList.Reset();
}

