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
	// not time
	if (!UImguiGlobalContextService::Get().TimeToDraw()) return;
	
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
		UClass* LowestClass = Objs[0]->GetClass();
		for (UObject* Obj : Objs)
		{
			while (!Obj->GetClass()->IsChildOf(LowestClass))
			{
				LowestClass = LowestClass->GetSuperClass();
			}
		}
		_DrawMultObj(DetailName, Objs, ChildBuilder, LowestClass);
	}
	
}

void FImguiDetailCustomization::_DrawMultObj(FName DetailName, const TArray<UObject*>& InObjs,
	IDetailChildrenBuilder& ChildBuilder, UClass* LowestClass)
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
    .GetAllDetailCustomizationOfClass(LowestClass);

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
        FName(LowestClass->GetName() + TEXT("Imgui")),
        FText::FromString(LowestClass->GetName() + TEXT("Imgui")),
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
	RenderProxy->SetPersistWndID(WndID);

	// add window 
	UImguiGlobalContextService::Get().GetGlobalContext()
    ->AddGlobalWindow(FDrawGlobalImgui::CreateLambda(
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
	UImguiGlobalContextService::Get().GetGlobalContext()->AddRenderProxy(RenderProxy);
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
	RenderProxy->SetPersistWndID(WndID);

	// add window 
	UImguiGlobalContextService::Get().GetGlobalContext()->AddGlobalWindow(FDrawGlobalImgui::CreateLambda(
		[Obj, OnGUIFunc, DetailName, RenderProxy, AllCustomization]
        {
			if (RenderProxy.IsUnique()) return false;
            UObject* GotObj = Obj.Get();
            if (!GotObj) return false;

			// set up detail info 
            ImGui::SetCurrentDetail(DetailName);
            ImGui::SetCurrentDetailWidget(RenderProxy);

			// call on gui 
			if (OnGUIFunc)
			{
				FFrame Stack(GotObj, OnGUIFunc, nullptr);
				OnGUIFunc->Invoke(GotObj, Stack, nullptr);
			}

			// call customization 
			for (UImguiDetailCustomization* Customization : AllCustomization)
			{
				Customization->OnEditSingleObject(GotObj);
			}
            return true;
        }));
	UImguiGlobalContextService::Get().GetGlobalContext()->AddRenderProxy(RenderProxy);
}
