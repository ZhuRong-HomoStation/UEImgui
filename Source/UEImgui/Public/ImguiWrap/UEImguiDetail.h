#pragma once

class IImguiStructDetailCustomizer;

class FUEImguiDetail
{
public:
	FUEImguiDetail();
	bool MakeDetail(UScriptStruct* InStruct, void* InValue);
	bool MakeDetail(UClass* InClass, void* InValue);
	bool MakeDetail(FProperty* InProperty, void* InContainer);
private:
	TMap<FString, TSharedPtr<IImguiStructDetailCustomizer>>		StructCustomizerMap;
	uint32			DetailDepth;
};

class IImguiStructDetailCustomizer
{
public:
	virtual ~IImguiStructDetailCustomizer() {}
	virtual bool Customize(FProperty* InProperty, void* InValue, UStruct* InOwner) = 0;
};


