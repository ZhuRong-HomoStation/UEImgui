#pragma once 
#include "CoreMinimal.h"
#include "ImguiInputAdapter.generated.h"

class UImguiContext;
struct ImGuiIO;

UCLASS(CustomConstructor)
class UEIMGUI_API UImguiInputAdapter : public UObject
{
	GENERATED_BODY()
public:
	UImguiInputAdapter(const FObjectInitializer& InInitializer);
	
	// Key mapping
	static void CopyUnrealKeyMap(ImGuiIO* InIO);
	static uint32 MapKey(uint32 InKeyCode)
	{
		return InKeyCode < 512 ? InKeyCode : 256 + (InKeyCode % 256);
	}
	static uint32 MapKey(const FKey& InKey);
	static uint32 MapKey(const FKeyEvent& InKey)
	{
		return MapKey(InKey.GetKeyCode());
	}

	// Mouse mapping
	static uint32 MapMouse(const FPointerEvent& InMouseEvent);
	
	// Key input
	FReply OnKeyChar(const FCharacterEvent& InCharacterEvent);
	FReply OnKeyDown(const FKeyEvent& InKeyEvent);
	FReply OnKeyUp(const FKeyEvent& InKeyEvent);

	// Mouse input
	FReply OnMouseButtonDown(const FPointerEvent& MouseEvent);
	FReply OnMouseButtonUp(const FPointerEvent& MouseEvent);
	FReply OnMouseButtonDoubleClick(const FPointerEvent& InMouseEvent);
	FReply OnMouseWheel(const FPointerEvent& MouseEvent);
	FReply OnMouseMove(SWidget* InWidget, const FGeometry& InGeometry, const FPointerEvent& MouseEvent);
	FReply OnMouseMove(FVector2D OffsetPos, const FPointerEvent& MouseEvent);
	
	// TODO: Touch input

	// TODO: Gamepad input

	// Cursor query
	virtual FCursorReply OnCursorQuery(const FPointerEvent& CursorEvent);
	
	// Set context
	UImguiContext* GetContext() const { return BoundContext; }
	void SetContext(UImguiContext* InContext) { BoundContext = InContext; }


	bool CanBlockInput() const { return bBlockInput; }
	bool CanReceiveMouseInput() const { return bReceiveMouseInput; }
	bool CanReceiveKeyboardInput() const { return bReceiveKeyboardInput; }
	bool CanReceiveGamePadInput() const { return bReceiveGamePadInput; }

protected:
	// key 
	virtual void AddInputCharacter(TCHAR InChar);
	virtual void SetKeyState(uint32 InKeyNum, bool InState);
	virtual void SetCtrl(bool InState);
	virtual void SetAlt(bool InState);
	virtual void SetShift(bool InState);

	// mouse 
	virtual void SetMouseBtnState(uint32 MouseBtnIndex, bool InState);
	virtual void SetMouseWheel(float MouseWheel);
	virtual void SetMousePos(FVector2D InMousePos);

private:
	UPROPERTY()
	uint8			bBlockInput:1;
	UPROPERTY()
	uint8			bReceiveMouseInput:1;
	UPROPERTY()
	uint8			bReceiveKeyboardInput:1;
	UPROPERTY()
	uint8			bReceiveGamePadInput:1;
	UPROPERTY()
	UImguiContext*	BoundContext;
};
