#pragma once
#include "CoreMinimal.h"
#include "imgui.h"
#include "ImguiInputAdapter.h"
#include "ImguiInputAdapterDeferred.generated.h"

UCLASS(CustomConstructor)
class UEIMGUI_API UImguiInputAdapterDeferred : public UImguiInputAdapter
{
	GENERATED_BODY()
public:
	UImguiInputAdapterDeferred(const FObjectInitializer& InInitializer);
	
	// ~Begin UImguiInputAdapter API 
	virtual void AddInputCharacter(TCHAR InChar) override;
	virtual void SetKeyState(uint32 InKeyNum, bool InState) override;
	virtual void SetCtrl(bool InState) override;
	virtual void SetAlt(bool InState) override;
	virtual void SetShift(bool InState) override;

	virtual void SetMouseBtnState(uint32 MouseBtnIndex, bool InState) override;
	virtual void SetMouseWheel(float InMouseWheel) override;
	virtual void SetMousePos(FVector2D InMousePos) override;

	virtual FCursorReply OnCursorQuery(const FPointerEvent& CursorEvent);
	// ~End UImguiInputAdapter API 

	void ApplyInput();
	void SaveTempData();

private:
	// Key chars 
	ImVector<ImWchar> InputQueueCharacters;

	// Modifier key
	bool        KeyCtrl; 
	bool        KeyShift;
	bool        KeyAlt;

	// Key downs 
	bool        KeysDown[512]; 

	// Mouse downs 
	bool        MouseDown[5];

	// Mouse wheel 
	float       MouseWheel;

	// Mouse pos
	ImVec2      MousePos;

	// Mouse cursor
	ImGuiMouseCursor	Cursor;
};


