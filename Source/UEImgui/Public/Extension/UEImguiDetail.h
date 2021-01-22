#pragma once

class FUEImguiDetail
{
public:
	FUEImguiDetail();
	bool MakeDetail(UScriptStruct* InStruct, void* InValue);
	bool MakeDetail(UClass* InClass, void* InValue);
	bool MakeDetail(FProperty* InProperty, void* InContainer);
private:
	uint32			DetailDepth;
};


