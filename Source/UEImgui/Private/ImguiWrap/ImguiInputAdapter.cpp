#include "ImguiWrap/ImguiInputAdapter.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "ImguiWrap/ImguiContext.h"

UImguiInputAdapter::UImguiInputAdapter(const FObjectInitializer& InInitializer)
	: Super(InInitializer)
	, bBlockInput(true)
	, bReceiveMouseInput(true)
	, bReceiveKeyboardInput(true)
	, bReceiveGamePadInput(true)
	, BoundContext(nullptr)
{
	
}

void UImguiInputAdapter::CopyUnrealKeyMap(ImGuiIO* InIO)
{
	static int KeyMap[ImGuiKey_COUNT];
	static bool bHasSetKeyMap = false;
	if (!bHasSetKeyMap)
	{
		KeyMap[ImGuiKey_Tab] = 			MapKey(EKeys::Tab);
		KeyMap[ImGuiKey_LeftArrow] = 	MapKey(EKeys::Left);
		KeyMap[ImGuiKey_RightArrow] = 	MapKey(EKeys::Right);
		KeyMap[ImGuiKey_UpArrow] = 		MapKey(EKeys::Up);
		KeyMap[ImGuiKey_DownArrow] = 	MapKey(EKeys::Down);
		KeyMap[ImGuiKey_PageUp] = 		MapKey(EKeys::PageUp);
		KeyMap[ImGuiKey_PageDown] =	 	MapKey(EKeys::PageDown);
		KeyMap[ImGuiKey_Home] = 		MapKey(EKeys::Home);
		KeyMap[ImGuiKey_End] = 			MapKey(EKeys::End);
		KeyMap[ImGuiKey_Insert] = 		MapKey(EKeys::Insert);
		KeyMap[ImGuiKey_Delete] = 		MapKey(EKeys::Delete);
		KeyMap[ImGuiKey_Backspace] = 	MapKey(EKeys::BackSpace);
		KeyMap[ImGuiKey_Space] = 		MapKey(EKeys::SpaceBar);
		KeyMap[ImGuiKey_Enter] = 		MapKey(EKeys::Enter);
		KeyMap[ImGuiKey_Escape] = 		MapKey(EKeys::Escape);
		KeyMap[ImGuiKey_A] = 			MapKey(EKeys::A);
		KeyMap[ImGuiKey_C] = 			MapKey(EKeys::C);
		KeyMap[ImGuiKey_V] = 			MapKey(EKeys::V);
		KeyMap[ImGuiKey_X] = 			MapKey(EKeys::X);
		KeyMap[ImGuiKey_Y] = 			MapKey(EKeys::Y);
		KeyMap[ImGuiKey_Z] = 			MapKey(EKeys::Z);
		bHasSetKeyMap = true;
	}
	FMemory::Memcpy(InIO->KeyMap, KeyMap, sizeof(int) * ImGuiKey_COUNT);
}

uint32 UImguiInputAdapter::MapKey(const FKey& InKey)
{
	const uint32* pKeyCode = nullptr;
	const uint32* pCharCode = nullptr;

	FInputKeyManager::Get().GetCodesFromKey(InKey, pKeyCode, pCharCode);

	const uint32 KeyCode =
        pKeyCode ? *pKeyCode
        : pCharCode ? *pCharCode
        : 0;

	return MapKey(KeyCode);
}

uint32 UImguiInputAdapter::MapMouse(const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return ImGuiMouseButton_Left;
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::MiddleMouseButton)
	{
		return ImGuiMouseButton_Middle;
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		return ImGuiMouseButton_Right;
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::ThumbMouseButton)
	{
		return ImGuiMouseButton_Right + 1;
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::ThumbMouseButton2)
	{
		return ImGuiMouseButton_Right + 2;		
	}
	return INDEX_NONE;
}

FReply UImguiInputAdapter::OnKeyChar(const FCharacterEvent& InCharacterEvent)
{
	if (!bReceiveKeyboardInput || !BoundContext) return FReply::Unhandled();
	AddInputCharacter(InCharacterEvent.GetCharacter());
	return bBlockInput && BoundContext->GetIO()->WantCaptureKeyboard ? FReply::Handled() : FReply::Unhandled();
}

FReply UImguiInputAdapter::OnKeyDown(const FKeyEvent& InKeyEvent)
{
	if (!bReceiveKeyboardInput || !BoundContext) return FReply::Unhandled();

	// Modifier keys 
	SetCtrl(InKeyEvent.IsControlDown());
	SetShift(InKeyEvent.IsShiftDown());
	SetAlt(InKeyEvent.IsAltDown());

	// Add key down input
	if (InKeyEvent.GetKey().IsGamepadKey())
	{
		
	}
	else if (InKeyEvent.GetKey().IsTouch())
	{
		
	}
	else
	{
		SetKeyState(MapKey(InKeyEvent), true);
	}
	return bBlockInput && BoundContext->GetIO()->WantCaptureKeyboard ? FReply::Handled() : FReply::Unhandled();
}

FReply UImguiInputAdapter::OnKeyUp(const FKeyEvent& InKeyEvent)
{
	if (!bReceiveKeyboardInput || !BoundContext) return FReply::Unhandled();

	// Modifier keys 
	SetCtrl(InKeyEvent.IsControlDown());
	SetShift(InKeyEvent.IsShiftDown());
	SetAlt(InKeyEvent.IsAltDown());

	// Add key down input
	if (InKeyEvent.GetKey().IsGamepadKey())
	{
		
	}
	else if (InKeyEvent.GetKey().IsTouch())
	{
		
	}
	else
	{
		SetKeyState(MapKey(InKeyEvent), false);
	}
	return bBlockInput && BoundContext->GetIO()->WantCaptureKeyboard ? FReply::Handled() : FReply::Unhandled();
}

FReply UImguiInputAdapter::OnMouseButtonDown(const FPointerEvent& MouseEvent)
{
	if (!bReceiveMouseInput || !BoundContext) return FReply::Unhandled();

	uint32 Index = MapMouse(MouseEvent);
	if (Index == INDEX_NONE) return FReply::Unhandled();

	SetMouseBtnState(Index, true);
	return bBlockInput && BoundContext->GetIO()->WantCaptureMouse ? FReply::Handled() : FReply::Unhandled();	
}

FReply UImguiInputAdapter::OnMouseButtonUp(const FPointerEvent& MouseEvent)
{
	if (!bReceiveMouseInput || !BoundContext) return FReply::Unhandled();

	uint32 Index = MapMouse(MouseEvent);
	if (Index == INDEX_NONE) return FReply::Unhandled();

	SetMouseBtnState(Index, false);
	return bBlockInput && BoundContext->GetIO()->WantCaptureMouse ? FReply::Handled() : FReply::Unhandled();	
}

FReply UImguiInputAdapter::OnMouseButtonDoubleClick(const FPointerEvent& InMouseEvent)
{
	if (!bReceiveMouseInput || !BoundContext) return FReply::Unhandled();

	uint32 Index = MapMouse(InMouseEvent);
	if (Index == INDEX_NONE) return FReply::Unhandled();

	SetMouseBtnState(Index, true);
	return bBlockInput && BoundContext->GetIO()->WantCaptureMouse ? FReply::Handled() : FReply::Unhandled();	
}

FReply UImguiInputAdapter::OnMouseWheel(const FPointerEvent& MouseEvent)
{
	if (!bReceiveMouseInput || !BoundContext) return FReply::Unhandled();

	SetMouseWheel(MouseEvent.GetWheelDelta());
	return bBlockInput && BoundContext->GetIO()->WantCaptureMouse ? FReply::Handled() : FReply::Unhandled();	
}

FReply UImguiInputAdapter::OnMouseMove(SWidget* InWidget, const FGeometry& InGeometry, const FPointerEvent& MouseEvent)
{
	if (!bReceiveMouseInput || !BoundContext) return FReply::Unhandled();

	FVector2D Position = MouseEvent.GetScreenSpacePosition();
	Position = InGeometry.GetAccumulatedRenderTransform().Inverse().TransformPoint(Position);
	
	SetMousePos(Position);
	
	return bBlockInput && BoundContext->GetIO()->WantCaptureMouse ? FReply::Handled() : FReply::Unhandled();	
}

FReply UImguiInputAdapter::OnMouseMove(FVector2D OffsetPos, const FPointerEvent& MouseEvent)
{
	if (!bReceiveMouseInput || !BoundContext) return FReply::Unhandled();

	SetMousePos(MouseEvent.GetScreenSpacePosition() - OffsetPos);
	
	return bBlockInput && BoundContext->GetIO()->WantCaptureMouse ? FReply::Handled() : FReply::Unhandled();	
}

FCursorReply UImguiInputAdapter::OnCursorQuery(const FPointerEvent& CursorEvent)
{
	if (!BoundContext) return FCursorReply::Unhandled();
	
	switch (BoundContext->GetContext()->MouseCursor)
	{
	case ImGuiMouseCursor_Arrow:       return FCursorReply::Cursor(EMouseCursor::Default);
	case ImGuiMouseCursor_TextInput:   return FCursorReply::Cursor(EMouseCursor::TextEditBeam);
	case ImGuiMouseCursor_ResizeAll:   return FCursorReply::Cursor(EMouseCursor::CardinalCross);
	case ImGuiMouseCursor_ResizeEW:    return FCursorReply::Cursor(EMouseCursor::ResizeLeftRight);
	case ImGuiMouseCursor_ResizeNS:    return FCursorReply::Cursor(EMouseCursor::ResizeUpDown);
	case ImGuiMouseCursor_ResizeNESW:  return FCursorReply::Cursor(EMouseCursor::ResizeSouthWest);
	case ImGuiMouseCursor_ResizeNWSE:  return FCursorReply::Cursor(EMouseCursor::ResizeSouthEast);
	case ImGuiMouseCursor_Hand:        return FCursorReply::Cursor(EMouseCursor::Hand);
	case ImGuiMouseCursor_NotAllowed:  return FCursorReply::Cursor(EMouseCursor::SlashedCircle);
	}
	return FCursorReply::Cursor(EMouseCursor::Default);
}

void UImguiInputAdapter::AddInputCharacter(TCHAR InChar)
{
	BoundContext->GetIO()->AddInputCharacter(static_cast<ImWchar>(InChar));
}

void UImguiInputAdapter::SetKeyState(uint32 InKeyNum, bool InState)
{
	BoundContext->GetIO()->KeysDown[InKeyNum] = InState;
}

void UImguiInputAdapter::SetCtrl(bool InState)
{
	BoundContext->GetIO()->KeyCtrl = InState;
}

void UImguiInputAdapter::SetAlt(bool InState)
{
	BoundContext->GetIO()->KeyAlt = InState;
}

void UImguiInputAdapter::SetShift(bool InState)
{
	BoundContext->GetIO()->KeyShift = InState;
}

void UImguiInputAdapter::SetMouseBtnState(uint32 MouseBtnIndex, bool InState)
{
	BoundContext->GetIO()->MouseDown[MouseBtnIndex] = InState;
}

void UImguiInputAdapter::SetMouseWheel(float MouseWheel)
{
	BoundContext->GetIO()->MouseWheel = MouseWheel;
}

void UImguiInputAdapter::SetMousePos(FVector2D InMousePos)
{
	BoundContext->GetIO()->MousePos.x = InMousePos.X;
	BoundContext->GetIO()->MousePos.y = InMousePos.Y;
}
