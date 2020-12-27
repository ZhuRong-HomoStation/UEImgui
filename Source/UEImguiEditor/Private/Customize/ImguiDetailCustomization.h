#pragma once
#include "IDetailCustomization.h"

class FImguiDetailCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

protected:
	// Begin IDetailCustomization API
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils ) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils ) override;
	// End IDetailCustomization API

private:
	void _DrawMultObj(FName DetailName, const TArray<UObject*>& InObjs, IDetailChildrenBuilder& ChildBuilder, UClass* LowestClass);
	void _DrawSingleObj(FName DetailName, const TArray<UObject*>& InObjs, IDetailChildrenBuilder& ChildBuilder);
};
