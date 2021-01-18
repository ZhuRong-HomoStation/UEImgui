#include "UEImguiEditor.h"
#include "imgui.h"
#include "implot.h"
#include "LevelEditor.h"
#include "Logging.h"
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
	_InitMenu();

	ImPlotCtx = ImPlot::CreateContext();
}

void FUEImguiEditor::ShutdownModule()
{
	_ShutDownDetailExtension();
	_ShutDownMenu();
	ImPlot::DestroyContext(ImPlotCtx);
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

void FUEImguiEditor::_InitMenu()
{
	// Get level editor module 
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	auto ExtendMenu = [](FMenuBuilder& InBuilder)
	{
		InBuilder.AddMenuEntry(FText::FromString(TEXT("Open ImGui Demo")), FText::GetEmpty(), FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda([]
			{
				UEImGui::AddGlobalWindow(FDrawGlobalImgui::CreateLambda([]
                {
                    bool IsOpen = true;
                    ImGui::ShowDemoWindow(&IsOpen);
                    return IsOpen;
                }));
			})));

		InBuilder.AddMenuEntry(FText::FromString(TEXT("Open ImPlot Demo")), FText::GetEmpty(), FSlateIcon(),
                    FUIAction(FExecuteAction::CreateLambda([]
                    {
                        UEImGui::AddGlobalWindow(FDrawGlobalImgui::CreateLambda([]
                        {
                            bool IsOpen = true;
                            ImPlot::ShowDemoWindow(&IsOpen);
                            return IsOpen;
                        }));
                    })));
		
		InBuilder.AddMenuEntry(FText::FromString(TEXT("Open Style Editor")), FText::GetEmpty(), FSlateIcon(),
            FUIAction(FExecuteAction::CreateLambda([]
            {
            	UEImGui::AddGlobalWindow(FDrawGlobalImgui::CreateLambda([]
                 {
                     bool IsOpen = true;
                     ImGui::Begin("ImguiStyleEditor", &IsOpen);
                     ImGui::ShowUEStyleEditor();
                     ImGui::End();
                     return IsOpen;
                 }));	
            })));
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
        nullptr,
        FMenuBarExtensionDelegate::CreateLambda(ExtendMenuBar));
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
}

void FUEImguiEditor::_ShutDownMenu()
{
}

void FUEImguiEditor::_ExtendMenu(FMenuBuilder& InBuilder)
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUEImguiEditor, UEImguiEditor);