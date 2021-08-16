#pragma once
#include "CoreMinimal.h"
#include "Services/ImguiDetailCustomization.h"
#include "Example.generated.h"

UENUM()
enum class ETestEnum : uint8
{
	A UMETA(DisplayName = "选项A"),
	B UMETA(DisplayName = "选项B"),
	C UMETA(DisplayName = "选项C"),
	D UMETA(DisplayName = "选项D"),
	E UMETA(DisplayName = "选项E"),
	F UMETA(DisplayName = "选项F"),
};

USTRUCT()
struct FSimpleStruct
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int8		PropInt8;
	UPROPERTY(EditAnywhere)
	int16		PropInt16;
	UPROPERTY(EditAnywhere)
	int32		PropInt32;
	UPROPERTY(EditAnywhere)
	int64		PropInt64;
};

USTRUCT()
struct FTestStruct
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int8			PropInt8;
	UPROPERTY(EditAnywhere)
	uint8			PropUInt8;
	UPROPERTY(EditAnywhere)
	int16			PropInt16;
	UPROPERTY(EditAnywhere)
	uint16			PropUInt16;
	UPROPERTY(EditAnywhere)
	int32			PropInt32;
	UPROPERTY(EditAnywhere)
	uint32			PropUInt32;
	UPROPERTY(EditAnywhere)
	int64			PropInt64;
	UPROPERTY(EditAnywhere)
	uint64			PropUInt64;
	UPROPERTY(EditAnywhere)
	float			PropFloat;
	UPROPERTY(EditAnywhere)
	double			PropDouble;
	UPROPERTY(EditAnywhere)
	bool			PropBool;

	UPROPERTY(EditAnywhere)
	ETestEnum		PropEnum;
	UPROPERTY(EditAnywhere)
	FSimpleStruct	PropSimpleStruct;

	UPROPERTY(EditAnywhere)
	FName			PropName;
	UPROPERTY(EditAnywhere)
	FText			PropText;
	UPROPERTY(EditAnywhere)
	FString			PropString;

	UPROPERTY(EditAnywhere)
	FVector2D		PropVec2;
	UPROPERTY(EditAnywhere)
	FVector			PropVec3;
	UPROPERTY(EditAnywhere)
	FVector			PropVec4;
	UPROPERTY(EditAnywhere)
	FRotator		PropRotator;	
	UPROPERTY(EditAnywhere)
	FTransform		PropTransform;
	UPROPERTY(EditAnywhere)
	FColor			PropColor;
	UPROPERTY(EditAnywhere)
	FLinearColor	PropColorHDR;

	UPROPERTY(EditAnywhere)
	TArray<int32>	PropInt;
	UPROPERTY(EditAnywhere)
	TArray<FSimpleStruct>	PropStruct;	
	UPROPERTY(EditAnywhere)
	TMap<int32, FSimpleStruct>	PropMap;

	UPROPERTY(EditAnywhere)
	UObject*		PropObject;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UObject>	PropSoftObject;
	UPROPERTY(EditAnywhere)
	TLazyObjectPtr<UObject> PropLazyObject;	
	UPROPERTY(EditAnywhere)
	TWeakObjectPtr<UObject>	PropWeakObject;
	
	UPROPERTY(EditAnywhere)
	UClass*			PropClass;
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AActor>	PropSoftClassPtr;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor>		PropSubClassOf;	
};

UCLASS()
class AImguiExampleActor : public AActor
{
	GENERATED_BODY()
public:
	AImguiExampleActor();

	// open imgui demo window(use global context)
	void OpenImguiDemoGlobal();
	
	// open imgui demo dock window(use global context)
	void OpenImguiDemoDockGlobal();
	
	// open imgui demo window(use stand alone context)
	void OpenImguiDemoStandAlone();
	
	// use OnGUI function for customize detail panel 
	UFUNCTION()
    void OnGUI();

protected:
	UPROPERTY(EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, Category="0")
	FTestStruct		StructValue;

	UPROPERTY(EditAnywhere, Category="0")
	ETestEnum		EnumValue;

	bool			bGlobalWndAdded = false;	
};

UCLASS()
class UImguiDetailCustomizationExample : public UImguiDetailCustomization
{
	GENERATED_BODY()
public:
	// target class 
	virtual UClass* GetSupportClass() override { return UObject::StaticClass(); }

	// support single object editing 
	virtual bool SupportSingleObjectEditing() override { return true; }
	virtual void OnEditSingleObject(UObject* InObject) override;

	// support mult object editing 
	virtual bool SupportMultiObjectEditing() override { return true; }
	virtual void OnEditMultiObject(TArray<UObject*> InObjects) override;
};

