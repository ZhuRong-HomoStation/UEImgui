#include "ImguiWrap/UEImguiDetail.h"
#include "imgui.h"
#include "ImguiWrap/ImguiUEWrap.h"

FUEImguiDetail::FUEImguiDetail()
	: DetailDepth(0)
{
}

bool FUEImguiDetail::MakeDetail(UScriptStruct* InStruct, void* InValue)
{
	bool bHasChanged = false;
	++DetailDepth;
	if (ImGui::BeginTable("ImGuiDetail", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingPolicyStretch))
	{
		for (TFieldIterator<FProperty> It(InStruct); It; ++It)
		{
			bHasChanged |= MakeDetail(*It, InValue);
		}
		ImGui::EndTable();
	}
	--DetailDepth;
	return bHasChanged;
}

bool FUEImguiDetail::MakeDetail(UClass* InClass, void* InValue)
{
	bool bHasChanged = false;
	++DetailDepth;

	--DetailDepth;
	return bHasChanged;
}

bool FUEImguiDetail::MakeDetail(FProperty* InProperty, void* InContainer)
{
	++DetailDepth;
	
	// constant 
	const char  s8_min  = -128,			s8_max = 127;
	const ImU8  u8_min  = 0,			u8_max = 255;
	const short s16_min = -32768,		s16_max = 32767;
	const ImU16 u16_min = 0,			u16_max = 65535;
	const ImS32 s32_min = INT_MIN,		s32_max = INT_MAX;
	const ImU32 u32_min = 0,			u32_max = UINT_MAX;
	const ImS64 s64_min = LLONG_MIN,	s64_max = LLONG_MAX;
	const ImU64 u64_min = 0,			u64_max = ULLONG_MAX;

	// string buffer
	static std::string StrBuffer;
	
	// get data 
	void* PropertyData = InProperty->ContainerPtrToValuePtr<void>(InContainer);
	bool bValueHasChanged = false;
	
	// draw label
	std::string PropertyName = TCHAR_TO_UTF8(*InProperty->GetName());
	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::AlignTextToFramePadding();
	ImGui::Text(PropertyName.c_str());
	ImGui::TableSetColumnIndex(1);

	// setup label
	PropertyName = "##" + PropertyName;

	// setup size
	ImGui::SetNextItemWidth(-FLT_MIN);
	
	// draw value 
	switch ((EClassCastFlags)InProperty->GetClass()->GetId())
	{
	// ========================Integer========================
	case CASTCLASS_FInt8Property:
		bValueHasChanged = ImGui::DragScalar(PropertyName.c_str(), ImGuiDataType_S8, PropertyData, 1.f, &s8_min, &s8_max);
		break;
	case CASTCLASS_FByteProperty:
		bValueHasChanged = ImGui::DragScalar(PropertyName.c_str(), ImGuiDataType_U8, PropertyData, 1.f, &u8_min, &u8_max);
		break;
	case CASTCLASS_FInt16Property:
		bValueHasChanged = ImGui::DragScalar(PropertyName.c_str(), ImGuiDataType_S16, PropertyData, 1.f, &s16_min, &s16_max);
		break;
	case CASTCLASS_FUInt16Property:
		bValueHasChanged = ImGui::DragScalar(PropertyName.c_str(), ImGuiDataType_U16, PropertyData, 1.f, &u16_min, &u16_max);
		break;
	case CASTCLASS_FIntProperty:
		bValueHasChanged = ImGui::DragScalar(PropertyName.c_str(), ImGuiDataType_S32, PropertyData, 1.f, &s32_min, &s32_max);
		break;
	case CASTCLASS_FUInt32Property:
		bValueHasChanged = ImGui::DragScalar(PropertyName.c_str(), ImGuiDataType_U32, PropertyData, 1.f, &u32_min, &u32_max);
		break;
	case CASTCLASS_FInt64Property:
		bValueHasChanged = ImGui::DragScalar(PropertyName.c_str(), ImGuiDataType_S64, PropertyData, 1.f, &s64_min, &s64_max);
		break;
	case CASTCLASS_FUInt64Property:
        bValueHasChanged = ImGui::DragScalar(PropertyName.c_str(), ImGuiDataType_U64, PropertyData, 1.f, &u64_min, &u64_max);
		break;

	// ========================Floating========================
	case CASTCLASS_FFloatProperty:
		bValueHasChanged = ImGui::DragScalar(PropertyName.c_str(), ImGuiDataType_Float, PropertyData, 1.f);
		break;
	case CASTCLASS_FDoubleProperty:
		bValueHasChanged = ImGui::DragScalar(PropertyName.c_str(), ImGuiDataType_Double, PropertyData, 1.f);
		break;

	// ========================Boolean========================		
	case CASTCLASS_FBoolProperty:
		bValueHasChanged = ImGui::Checkbox(PropertyName.c_str(), (bool*)PropertyData);
		break;
		
	// ========================String========================
	case CASTCLASS_FNameProperty:
	{
		FName& NameValue = *(FName*)PropertyData;
		FString NameStr = NameValue.ToString();
		int32 UTF8Len = FTCHARToUTF8_Convert::ConvertedLength(*NameStr, NameStr.Len());
		StrBuffer.resize(UTF8Len + 1);
		StrBuffer.back() = 0;
		FTCHARToUTF8_Convert::Convert(const_cast<char*>(StrBuffer.data()), StrBuffer.length() + 1, *NameStr, NameStr.Len());
		bValueHasChanged = ImGui::InputText(PropertyName.c_str(), &StrBuffer);
		if (bValueHasChanged)
		{
			NameValue = FName(UTF8_TO_TCHAR(StrBuffer.c_str()));
		}
		break;
	}
	case CASTCLASS_FTextProperty:
	{
		FText& TextValue = *(FText*)PropertyData;
		FString NameStr = TextValue.ToString();
		int32 UTF8Len = FTCHARToUTF8_Convert::ConvertedLength(*NameStr, NameStr.Len());
		StrBuffer.resize(UTF8Len + 1);
		StrBuffer.back() = 0;
		FTCHARToUTF8_Convert::Convert(const_cast<char*>(StrBuffer.data()), StrBuffer.length() + 1, *NameStr, NameStr.Len());
		bValueHasChanged = ImGui::InputText(PropertyName.c_str(), &StrBuffer);
		if (bValueHasChanged)
		{
			int32 TCHARLen = FUTF8ToTCHAR_Convert::ConvertedLength(StrBuffer.c_str(), StrBuffer.length());
			NameStr.GetCharArray().SetNumUninitialized(TCHARLen + 1);
			NameStr.GetCharArray().Top() = 0;
			FUTF8ToTCHAR_Convert::Convert(NameStr.GetCharArray().GetData(), NameStr.GetCharArray().Num(), StrBuffer.c_str(), StrBuffer.length());
			TextValue = FText::FromString(NameStr);
		}
		break;
	}
	case CASTCLASS_FStrProperty:
	{
		FString& NameStr = *(FString*)PropertyData;
		int32 UTF8Len = FTCHARToUTF8_Convert::ConvertedLength(*NameStr, NameStr.Len());
		StrBuffer.resize(UTF8Len + 1);
		StrBuffer.back() = 0;
		FTCHARToUTF8_Convert::Convert(const_cast<char*>(StrBuffer.data()), StrBuffer.length() + 1, *NameStr, NameStr.Len());
		bValueHasChanged = ImGui::InputText(PropertyName.c_str(), &StrBuffer);
		if (bValueHasChanged)
		{
			int32 TCHARLen = FUTF8ToTCHAR_Convert::ConvertedLength(StrBuffer.c_str(), StrBuffer.length());
			NameStr.GetCharArray().SetNumUninitialized(TCHARLen + 1);
			NameStr.GetCharArray().Top() = 0;
			FUTF8ToTCHAR_Convert::Convert(NameStr.GetCharArray().GetData(), NameStr.GetCharArray().Num(), StrBuffer.c_str(), StrBuffer.length());
		}
		break;
	}

	// ========================Object========================
	case CASTCLASS_FObjectProperty:
	case CASTCLASS_FWeakObjectProperty:
	case CASTCLASS_FLazyObjectProperty:
	case CASTCLASS_FSoftObjectProperty:
		break;

	// ========================Class========================
	case CASTCLASS_FClassProperty:
	case CASTCLASS_FSoftClassProperty:
		break;
		
	// ========================Combo========================
	case CASTCLASS_FEnumProperty:
	{
		FEnumProperty* EnumProperty = CastField<FEnumProperty>(InProperty);
		EnumProperty->GetClass()->GetId();
		FNumericProperty* LocalUnderlyingProp = EnumProperty->GetUnderlyingProperty();
		int64 Value = LocalUnderlyingProp->GetSignedIntPropertyValue(PropertyData);
		bValueHasChanged = ImGui::UEEnum(PropertyName.c_str(), EnumProperty->GetEnum(), &Value);
		LocalUnderlyingProp->SetIntPropertyValue(PropertyData, Value);
		break;
	}
	case CASTCLASS_FStructProperty:
	{
		FStructProperty* StructProperty = CastField<FStructProperty>(InProperty);
		bValueHasChanged = ImGui::UEStruct(StructProperty->Struct, PropertyData);
		break;
	}

	// ========================Misc======================== 
	case CASTCLASS_FArrayProperty:
	case CASTCLASS_FMapProperty:
	case CASTCLASS_FSetProperty:
		break;

	// ========================AActor========================
	case CASTCLASS_AActor:
	case CASTCLASS_APlayerController:
	case CASTCLASS_APawn:
		break;

	// ========================Reflection[Unused]========================
	case CASTCLASS_UField:
	case CASTCLASS_UStruct:
	case CASTCLASS_UScriptStruct:
	case CASTCLASS_UClass:
	case CASTCLASS_UEnum:
	case CASTCLASS_UFunction:
		
	// ========================BasicProperty[Unused]========================
	case CASTCLASS_None:
	case CASTCLASS_FProperty:
	case CASTCLASS_FInterfaceProperty:
	case CASTCLASS_FNumericProperty:
	case CASTCLASS_FObjectPropertyBase:
	case CASTCLASS_FFieldPathProperty:

	// ========================Delegate[Unused]========================
	case CASTCLASS_FDelegateProperty:
	case CASTCLASS_FMulticastDelegateProperty:
	case CASTCLASS_FMulticastInlineDelegateProperty:
	case CASTCLASS_FMulticastSparseDelegateProperty:

	// ========================EngineMisc[Unused]========================
	case CASTCLASS_UPackage:
	case CASTCLASS_ULevel:
	case CASTCLASS_USceneComponent:
	case CASTCLASS_UPrimitiveComponent:
	case CASTCLASS_USkinnedMeshComponent:
	case CASTCLASS_USkeletalMeshComponent:
	case CASTCLASS_UBlueprint:
	case CASTCLASS_UDelegateFunction:
	case CASTCLASS_UStaticMeshComponent:
	case CASTCLASS_USparseDelegateFunction:
	default:
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1,0,0,1));
		ImGui::Text("UnSupport");
		ImGui::PopStyleColor();
		break;
	}

	--DetailDepth;
	return bValueHasChanged;
}
