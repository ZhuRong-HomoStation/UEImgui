#include "UEImguiEditor.h"
#include "imgui.h"
#include "LevelEditor.h"
#include "Logging.h"
#include "Commands/ImguiEditorCommand.h"
#include "Customize/ImguiDetailCustomization.h"
#include "ImguiWrap/ImguiUEWrap.h"
#include "Modules/ModuleManager.h"
#include "Service/ImguiCustomDetailService.h"
#include "Services/ImguiGlobalContextService.h"
#include "Widgets/SImguiWidget.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SEditableText.h"

#define LOCTEXT_NAMESPACE "FUEImguiEditor"
class FLevelEditorModule;

void FUEImguiEditor::StartupModule()
{
	_InitDetailExtension();
	_InitCommand();
	_InitMenu();
}

void FUEImguiEditor::ShutdownModule()
{
	_ShutDownDetailExtension();
	_ShutDownCommand();
	_ShutDownMenu();
}

void FUEImguiEditor::_InitDetailExtension()
{
	if (!AActor::StaticClass()->FindPropertyByName(TEXT("ImguiDummy")))
	{
		FStructProperty* DummyProperty = new FStructProperty(AActor::StaticClass()
            , TEXT("ImguiDummy"), EObjectFlags::RF_NoFlags);
		DummyProperty->Struct = FImguiDummy::StaticStruct();
		DummyProperty->PropertyFlags |= EPropertyFlags::CPF_Transient;
		DummyProperty->PropertyFlags |= EPropertyFlags::CPF_EditConst;
		DummyProperty->PropertyFlags |= EPropertyFlags::CPF_Edit;
		DummyProperty->SetMetaData(TEXT("Category"), TEXT("Imgui"));
		AActor::StaticClass()->AddCppProperty(DummyProperty);
	}
	
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyEditorModule.RegisterCustomPropertyTypeLayout(
        TEXT("ImguiDummy"),
        FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FImguiDetailCustomization::MakeInstance));

	PropertyEditorModule.NotifyCustomizationModuleChanged();
}

void FUEImguiEditor::_ShutDownDetailExtension()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyEditorModule.UnregisterCustomPropertyTypeLayout("ImguiDummy");
	}
}

void FUEImguiEditor::_InitCommand()
{
	FImguiEditorCommand::Register();
	m_CmdList = MakeShareable(new FUICommandList);

	// open imgui demo window 
	m_CmdList->MapAction(
		FImguiEditorCommand::Get().OpenImguiDemo,
		FExecuteAction::CreateLambda([]
		{
			UImguiGlobalContextService::Get().AddGlobalWindow(FDrawGlobalImgui::CreateLambda([]
				{
					bool IsOpen = true;
					ImGui::ShowDemoWindow(&IsOpen);
					return IsOpen;
				}));			
		}),
		FCanExecuteAction());

	// open style editor 
	m_CmdList->MapAction(
        FImguiEditorCommand::Get().OpenStyleEditor,
        FExecuteAction::CreateLambda([]
        {
            UImguiGlobalContextService::Get().AddGlobalWindow(FDrawGlobalImgui::CreateLambda([]
                {
                    bool IsOpen = true;
            		ImGui::Begin("ImguiStyleEditor", &IsOpen);
                    ImGui::ShowUEStyleEditor();
            		ImGui::End();
                    return IsOpen;
                }));			
        }),
        FCanExecuteAction());
}

void FUEImguiEditor::_ShutDownCommand()
{
	FImguiEditorCommand::Unregister();
}

void FUEImguiEditor::_InitMenu()
{
	// Get level editor module 
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	auto ExtendMenu = [](FMenuBuilder& InBuilder)
	{
		InBuilder.AddMenuEntry(FImguiEditorCommand::Get().OpenImguiDemo);
		InBuilder.AddMenuEntry(FImguiEditorCommand::Get().OpenStyleEditor);
	};
	
	auto ExtendMenuBar = [&](FMenuBarBuilder& InBuilder)
	{
		InBuilder.AddPullDownMenu(
			FText::FromString(TEXT("Imgui")),
			FText::FromString(TEXT("Imgui")),
			FNewMenuDelegate::CreateLambda(ExtendMenu));
	};
	
	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
	MenuExtender->AddMenuBarExtension(
        "Help",
        EExtensionHook::After,
        m_CmdList,
        FMenuBarExtensionDelegate::CreateLambda(ExtendMenuBar));
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
}

void FUEImguiEditor::_ShutDownMenu()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUEImguiEditor, UEImguiEditor);