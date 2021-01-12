#pragma once
#include "ImguiWrap/ImguiContext.h"
#include "ImguiWrap/ImguiInputAdapterDeferred.h"

#include "ImguiPerInstanceCtx.generated.h"

UCLASS()
class UImguiPerInstanceCtx : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
public:
	UImguiContext* GetGlobalContext() const { return GlobalContext; }
protected:
	// ~Begin UGameInstanceSubsystem API
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// ~End UGameInstanceSubsystem API

	// ~Begin FTickableGameObject API 
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { return Super::GetStatID(); } 
	// ~End FTickableGameObject API

private:
	UPROPERTY()
	UImguiContext*		GlobalContext;

	UPROPERTY()
	UImguiInputAdapterDeferred*	InputAdapter;
};
