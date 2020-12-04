#pragma once
#include "CoreMinimal.h"
#include "ImguiContext.generated.h"

struct ImGuiStyle;
struct ImGuiIO;
struct ImGuiContext;
struct ImFontAtlas;

/*
 * hold an imgui environment 
 */
UCLASS(CustomConstructor)
class UEIMGUI_API UImguiContext : public UObject
{
	GENERATED_BODY()
protected:
	// ~Begin UObject API
	virtual void BeginDestroy() override;
	// ~End UObject API 
public:
	UImguiContext(const FObjectInitializer& InInitializer);

	void Init(ImFontAtlas* InDefaultFontAtlas = nullptr);
	void Reset();
	void ShutDown();
	
	// context operation 
	void ApplyContext();
	ImGuiContext* GetContext() const { return Context; }
	ImGuiIO* GetIO() const;
	ImGuiStyle* GetStyle() const;	
private:
	void _SetupImguiContext();
	void _SetUpDefaultFont();
	
private:
	ImGuiContext*		Context;
};
