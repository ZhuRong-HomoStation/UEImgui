#include "Example.h"
#include "ImguiWrap/ImguiUEWrap.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Services/ImguiGlobalContextService.h"
#include "Widgets/Docking/SDockTab.h"

AImguiExampleActor::AImguiExampleActor()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void AImguiExampleActor::OpenImguiDemoGlobal()
{
	UImguiGlobalContextService::Get().AddGlobalWindow(TEXT("Dear ImGui Demo"), FDrawGlobalImgui::CreateLambda([]
        {
            bool IsOpen = true;
            ImGui::ShowDemoWindow(&IsOpen);
            return IsOpen;
        }));
}

void AImguiExampleActor::OpenImguiDemoDockGlobal()
{
	// create an global imgui widget
	auto GlobalImguiWidget = SNew(SGlobalImguiWidget)
    .HSizingRule(EImguiSizingRule::UESize)
    .VSizingRule(EImguiSizingRule::UESize)
    .WndName(TEXT("Dear ImGui Demo"))
    .OnDraw(FOnImguiDraw::CreateLambda([]
    {
        ImGui::ShowDemoWindow();
    }));

	// create a dock tab 
	TSharedPtr<SDockTab> NewTab = SNew(SDockTab)
    .Label(FText::FromString(TEXT("Dear ImGui Demo")))
    .TabRole(ETabRole::NomadTab)
    [
		GlobalImguiWidget
    ];

	// register dock tab  	
	static FName PlaceholderId(TEXT("StandaloneToolkit"));
	static TSharedPtr<FTabManager::FSearchPreference> SearchPreference = MakeShareable(new FTabManager::FRequireClosedTab());
	FGlobalTabmanager::Get()->InsertNewDocumentTab(PlaceholderId, *SearchPreference, NewTab.ToSharedRef());
}

void AImguiExampleActor::OpenImguiDemoStandAlone()
{
	// create an stand alone imgui widget
	auto ImguiWidget = SNew(SImguiWidget)
	.OnDraw(FOnImguiDraw::CreateLambda([]
	{
		ImGui::ShowDemoWindow();
	}));

	// create a window
	auto NewWnd = SNew(SWindow)
	.Title(FText::FromString(TEXT("StandAloneImguiWindow")))
    .SizingRule(ESizingRule::UserSized)
	[
		ImguiWidget
	];

	// open window
	FSlateApplication::Get().AddWindow(NewWnd);
}

void AImguiExampleActor::OnGUI()
{
	ImGui::BeginDetail();
	ImGui::Columns(2);

	// simple functional 
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Simple functional: ");
	ImGui::NextColumn();
	if (ImGui::Button("SayHello"))
	{
		UKismetSystemLibrary::PrintString(GWorld);
	}
	ImGui::SameLine();
	if (ImGui::Button("Print Location"))
	{
		UKismetSystemLibrary::PrintString(GWorld, GetActorLocation().ToString());		
	}
	ImGui::NextColumn();

	// global context 
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Global context: ");
	ImGui::NextColumn();
	if (ImGui::Button("Open Demo"))
	{
		OpenImguiDemoGlobal();
	}
	ImGui::SameLine();
	if (ImGui::Button("Open Demo Dock"))
	{
		OpenImguiDemoDockGlobal();
	}
	ImGui::NextColumn();

	// stand alone context 
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Stand alone context: ");
	ImGui::NextColumn();
	if (ImGui::Button("Open Demo Window"))
	{
		OpenImguiDemoStandAlone();
	}
	ImGui::NextColumn();

	// resume 
	ImGui::Columns(1);

	// use input system 
	char buf[1024];
	ImGui::UseInputSystem("WDNMD");
	ImGui::InputText("WDNMD", buf, 1024);

	ImGui::EndDetail();
}

void UImguiDetailCustomizationExample::OnEditSingleObject(UObject* InObject)
{
	ImGui::BeginDetail();
	ImGui::Separator();
	ImGui::Text("This is single object editor");
	ImGui::EndDetail();
}

void UImguiDetailCustomizationExample::OnEditMultiObject(TArray<UObject*> InObjects)
{
	ImGui::BeginDetail();
	ImGui::Text("Edit multi object here");
	ImGui::Separator();
	for (UObject* Object : InObjects)
	{
		ImGui::Text(Object->GetName());
	}
	ImGui::EndDetail();
}
