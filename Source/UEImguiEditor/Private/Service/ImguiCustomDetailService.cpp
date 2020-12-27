#include "ImguiCustomDetailService.h"
#include "GenericPlatform/ITextInputMethodSystem.h"
#include "Services/ImguiDetailCustomization.h"

TArray<UImguiDetailCustomization*> UImguiCustomDetailService::GetAllDetailCustomizationOfClass(UClass* InClass)
{
	TArray<UImguiDetailCustomization*> AllCustomization;
	for (UClass* Class : AllDetailCustomization)
	{
		UImguiDetailCustomization* CDO = (UImguiDetailCustomization*)Class->GetDefaultObject(true);
		if (InClass->IsChildOf(CDO->GetSupportClass()))
		{
			AllCustomization.Add(CDO);
		}
	}
	return AllCustomization;
}

void UImguiCustomDetailService::Initialize(FSubsystemCollectionBase& Collection)
{
	GetDerivedClasses(UImguiDetailCustomization::StaticClass(), AllDetailCustomization);
}

void UImguiCustomDetailService::Deinitialize()
{
	// TODO: InputText 
//	FSlateApplication::Get().GetTextInputMethodSystem()->ActivateContext()
}
