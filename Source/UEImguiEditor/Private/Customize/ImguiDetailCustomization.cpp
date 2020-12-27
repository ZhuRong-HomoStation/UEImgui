#include "ImguiDetailCustomization.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "ImguiWrap/ImguiUEWrap.h"
#include "Service/ImguiCustomDetailService.h"
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

void FImguiDetailCustomization::CustomizeChildren(
	TSharedRef<IPropertyHandle> PropertyHandle,
	IDetailChildrenBuilder& ChildBuilder,
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	// get detail name 
	FName DetailName = ChildBuilder.GetParentCategory().GetParentLayout().GetDetailsView()->GetIdentifier();

	// find object and functions 
	TArray<UObject*> Objs;
	PropertyHandle->GetOuterObjects(Objs);

	if (Objs.Num() == 0) return;;
	if (Objs.Num() == 1)
	{
		_DrawSingleObj(DetailName, Objs, ChildBuilder);
	}
	else
	{
		UClass* CurrentClass = Objs[0]->GetClass();
		for (UObject* Obj : Objs)
		{
			if (Obj->GetClass() != CurrentClass)
			{
				return;
				break;
			}
		}
		_DrawMultObj(DetailName, Objs, ChildBuilder);
	}
	
}

void FImguiDetailCustomization::_DrawMultObj(FName DetailName, const TArray<UObject*>& InObjs,
	IDetailChildrenBuilder& ChildBuilder)
{
	// cast weak ptr
	TArray<TWeakObjectPtr<UObject>> AllObjs;
	AllObjs.Reserve(InObjs.Num());
	for (UObject* Obj : InObjs)
	{
		AllObjs.Emplace(Obj);
	}
	
	// find detail customization
	TArray<UImguiDetailCustomization*> AllCustomization = UImguiCustomDetailService::Get()
    .GetAllDetailCustomizationOfClass(InObjs[0]->GetClass());

	// cull not support multi object items 
	for (int32 i = 0; i < AllCustomization.Num();)
	{
		if (!AllCustomization[i]->SupportMultiObjectEditing())
		{
			AllCustomization.RemoveAtSwap(i);
		}
		else
		{
			++i;
		}
	}

	if (AllCustomization.Num() == 0) return;
	
	// edit category  
	IDetailCategoryBuilder& CategoryBuilder = ChildBuilder.GetParentCategory().GetParentLayout()
        .EditCategory(
        FName(InObjs[0]->GetClass()->GetName() + TEXT("Imgui")),
        FText::GetEmpty(),
        ECategoryPriority::Important);

	// set widget
	TSharedPtr<SImguiWidgetRenderProxy> RenderProxy;
	CategoryBuilder.AddCustomRow(FText::FromString(TEXT("Imgui")), false)
    .WholeRowWidget
    [
        SAssignNew(RenderProxy, SImguiWidgetRenderProxy)
        .HSizingRule(EImguiSizingRule::UESize)
        .VSizingRule(EImguiSizingRule::ImContentSize)
    ];
	ImGuiID WndID = ImHashStr(TCHAR_TO_UTF8(*DetailName.ToString()));
	RenderProxy->SetTopWnd(WndID);

	// add window 
	UImguiGlobalContextService::Get().AddGlobalWindow(DetailName.ToString(), FDrawGlobalImgui::CreateLambda(
        [DetailName, RenderProxy, AllCustomization, AllObjs]
        {
			if (RenderProxy.IsUnique()) return false;
            ImGui::SetCurrentDetail(DetailName);
            ImGui::SetCurrentDetailWidget(RenderProxy);
            for (UImguiDetailCustomization* Customization : AllCustomization)
            {
            	static TArray<UObject*> Objs;
            	Objs.Reset();
				for (auto & Obj : AllObjs)
				{
					auto GotObj = Obj.Get();
					if (GotObj)
					{
						Objs.Add(GotObj);
					}
				}
                Customization->OnEditMultiObject(Objs);
            }
            return true;
        }));
	UImguiGlobalContextService::Get().AddRenderProxy(RenderProxy);
}

void FImguiDetailCustomization::_DrawSingleObj(FName DetailName, const TArray<UObject*>& InObjs, IDetailChildrenBuilder& ChildBuilder)
{
	TWeakObjectPtr<UObject> Obj = InObjs[0];

	// find OnGUI
	UFunction* OnGUIFunc = Obj->GetClass()->FindFunctionByName(TEXT("OnGUI"));
	if (OnGUIFunc && (OnGUIFunc->NumParms != 0 || OnGUIFunc->ReturnValueOffset != 65535))
	{
		OnGUIFunc = nullptr;
	}

	// find detail customization
	TArray<UImguiDetailCustomization*> AllCustomization = UImguiCustomDetailService::Get()
	.GetAllDetailCustomizationOfClass(Obj->GetClass());

	// cull not support single object items 
	for (int32 i = 0; i < AllCustomization.Num();)
	{
		if (!AllCustomization[i]->SupportSingleObjectEditing())
		{
			AllCustomization.RemoveAtSwap(i);
		}
		else
		{
			++i;
		}
	}

	if (!OnGUIFunc && AllCustomization.Num() == 0) return;
	
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
	    .HSizingRule(EImguiSizingRule::UESize)
	    .VSizingRule(EImguiSizingRule::ImContentSize)
    ];
	ImGuiID WndID = ImHashStr(TCHAR_TO_UTF8(*DetailName.ToString()));
	RenderProxy->SetTopWnd(WndID);

	// add window 
	UImguiGlobalContextService::Get().AddGlobalWindow(DetailName.ToString(), FDrawGlobalImgui::CreateLambda(
		[Obj, OnGUIFunc, DetailName, RenderProxy, AllCustomization]
        {
			if (RenderProxy.IsUnique()) return false;
            UObject* GotObj = Obj.Get();
            if (!GotObj) return false;
            FFrame Stack(GotObj, OnGUIFunc, nullptr);
            ImGui::SetCurrentDetail(DetailName);
            ImGui::SetCurrentDetailWidget(RenderProxy);
            OnGUIFunc->Invoke(GotObj, Stack, nullptr);
			for (UImguiDetailCustomization* Customization : AllCustomization)
			{
				Customization->OnEditSingleObject(GotObj);
			}
            return true;
        }));
	UImguiGlobalContextService::Get().AddRenderProxy(RenderProxy);
}
