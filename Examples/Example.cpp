#include "Example.h"
#include "Misc/MessageDialog.h"
#include "ImguiWrap/ImguiUEWrap.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Services/ImguiGlobalContextService.h"
#include "Widgets/SImguiWidget.h"
#include "Widgets/Docking/SDockTab.h"

AImguiExampleActor::AImguiExampleActor()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void AImguiExampleActor::OpenImguiDemoGlobal()
{
	if (!bGlobalWndAdded)
	{
		bGlobalWndAdded = true;
		UEImGui::AddGlobalWindow(FDrawGlobalImgui::CreateLambda([This=TWeakObjectPtr<AImguiExampleActor>(this)]
	        {
				AImguiExampleActor* Actor = This.Get();
				if (!Actor) return false;
	            ImGui::ShowDemoWindow(&Actor->bGlobalWndAdded);
	            return Actor->bGlobalWndAdded;
	        }));
	}
}

void AImguiExampleActor::OpenImguiDemoDockGlobal()
{
	// Open Global Wnd
	OpenImguiDemoGlobal();
	
	// create an global imgui widget
	auto GlobalImguiWidget = SNew(SImguiRenderProxy)
    .HSizingRule(EImguiSizingRule::UESize)
    .VSizingRule(EImguiSizingRule::UESize)
    .ProxyWndName("Dear ImGui Demo");

	// register render proxy
	UEImGui::AddRenderProxy(GlobalImguiWidget, this);
	
	// create a dock tab 
	TSharedPtr<SDockTab> NewTab = SNew(SDockTab)
    .Label(FText::FromString(TEXT("Dear ImGui Demo")))
    .TabRole(ETabRole::NomadTab)
	.OnTabClosed_Lambda([This=TWeakObjectPtr<AImguiExampleActor>(this)](TSharedRef<SDockTab>){ if (This.IsValid()) This->bGlobalWndAdded = false; })
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
	// InProgress  
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
	// ImGui::AlignTextToFramePadding();
	// ImGui::Text("Stand alone context: ");
	// ImGui::NextColumn();
	// if (ImGui::Button("Open Demo Window"))
	// {
	// 	OpenImguiDemoStandAlone();
	// }
	// ImGui::NextColumn();

	// resume 
	ImGui::Columns(1);

	// enum 
	ImGui::UEEnum("EnumExample" ,&EnumValue);

	// struct
	if (ImGui::Button("Open Struct Detail"))
	{
		UEImGui::AddGlobalWindow(FDrawGlobalImgui::CreateLambda([this]()
		{
			bool bOpen = true;
			ImGui::SetNextWindowSize(ImVec2(600,800), ImGuiCond_Appearing);
			ImGui::Begin("Example Struct Detail", &bOpen);
			ImGui::UEStruct(&StructValue);
			ImGui::End();
			return bOpen;
		}));
	}

	ImGui::End();
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
