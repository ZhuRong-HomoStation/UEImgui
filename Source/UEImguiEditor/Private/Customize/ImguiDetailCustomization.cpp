#include "ImguiDetailCustomization.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "ImguiWrap/ImguiUEWrap.h"
#include "Services/ImguiGlobalContextService.h"
#include "Widgets/SImguiWidget.h"

TSharedRef<IPropertyTypeCustomization> FImguiDetailCustomization::MakeInstance()
{
	return MakeShareable(new FImguiDetailCustomization);
}

void FImguiDetailCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FImguiDetailCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	// get detail name 
	FName DetailName = ChildBuilder.GetParentCategory().GetParentLayout().GetDetailsView()->GetIdentifier();

	// find object and functions 
	TArray<UObject*> Objs;
	PropertyHandle->GetOuterObjects(Objs);
	if (Objs.Num() > 1) return;
	TWeakObjectPtr<UObject> Obj = Objs[0];
	UFunction* Func = Obj->GetClass()->FindFunctionByName(TEXT("OnGUI"));
	if (!Func) return;
	if (Func->NumParms != 0 || Func->ReturnValueOffset != 65535) return;

	// edit category  
	IDetailCategoryBuilder& CategoryBuilder = ChildBuilder.GetParentCategory().GetParentLayout()
		.EditCategory(
        FName(Obj->GetClass()->GetName() + TEXT("Imgui")),
        FText::GetEmpty(),
        ECategoryPriority::Important);

	// set widget
	TSharedPtr<SImguiWidgetRenderProxy> RenderProxy;
	CategoryBuilder.AddCustomRow(FText::FromString(TEXT("Imgui")), false)
    .WholeRowWidget
    [
        SAssignNew(RenderProxy, SImguiWidgetRenderProxy)
        .UseImguiWidgetWidth(false)
        .UseImguiWndContentSize(true)
    ];
	ImGuiID WndID = ImHashStr(TCHAR_TO_UTF8(*DetailName.ToString()));
	RenderProxy->SetTopWnd(WndID);
	TWeakPtr<SImguiWidgetRenderProxy> RenderProxyWP = RenderProxy;

	// add window 
	UImguiGlobalContextService::Get().AddGlobalWindow(FDrawGlobalImgui::CreateLambda([Obj, Func, DetailName, RenderProxyWP]
        {
            UObject* GotObj = Obj.Get();
            if (!GotObj) return false;
            FFrame Stack(GotObj, Func, nullptr);
            ImGui::SetCurrentDetail(DetailName);
			ImGui::SetCurrentDetailWidget(RenderProxyWP);
            Func->Invoke(GotObj, Stack, nullptr);
			return RenderProxyWP.IsValid();
        }));
	UImguiGlobalContextService::Get().AddRenderProxy(RenderProxyWP);
}
