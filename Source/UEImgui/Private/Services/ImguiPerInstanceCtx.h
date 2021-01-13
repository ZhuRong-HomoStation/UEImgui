#pragma once
#include "ImguiWrap/ImguiContext.h"
#include "ImguiWrap/ImguiInputAdapterDeferred.h"
#include "ImguiPerInstanceCtx.generated.h"

UCLASS()
class UImguiPerInstanceCtx : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UImguiContext* GetGlobalContext() const { return GlobalContext; }
	UImguiInputAdapterDeferred* GetGlobalInputAdapter() const { return InputAdapter; }
protected:
	// ~Begin UGameInstanceSubsystem API
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// ~End UGameInstanceSubsystem API 
	void Tick(float DeltaTime);

	virtual void BeginDestroy() override;
private:
	UPROPERTY()
	UImguiContext*		GlobalContext;

	UPROPERTY()
	UImguiInputAdapterDeferred*	InputAdapter;

	FDelegateHandle PreSlateTick;
};
