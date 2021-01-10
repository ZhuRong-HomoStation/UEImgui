#include "ImguiInput/ImguiInputAdapterDeferred.h"
#include "imgui_internal.h"

void UImguiInputAdapterDeferred::AddInputCharacter(TCHAR InChar)
{
	if (InChar != 0)
		InputQueueCharacters.push_back(InChar <= IM_UNICODE_CODEPOINT_MAX ? (ImWchar)InChar : IM_UNICODE_CODEPOINT_INVALID);
}

void UImguiInputAdapterDeferred::SetKeyState(uint32 InKeyNum, bool InState)
{
	KeysDown[InKeyNum] = InState;
}

void UImguiInputAdapterDeferred::SetCtrl(bool InState)
{
	KeyCtrl = InState;
}

void UImguiInputAdapterDeferred::SetAlt(bool InState)
{
	KeyAlt = InState;
}

void UImguiInputAdapterDeferred::SetShift(bool InState)
{
	KeyShift = InState;
}

void UImguiInputAdapterDeferred::SetMouseBtnState(uint32 MouseBtnIndex, bool InState)
{
	MouseDown[MouseBtnIndex] = InState;
}

void UImguiInputAdapterDeferred::SetMouseWheel(float InMouseWheel)
{
	MouseWheel = InMouseWheel;
}

void UImguiInputAdapterDeferred::SetMousePos(FVector2D InMousePos)
{
	MousePos.x = InMousePos.X;
	MousePos.y = InMousePos.Y;
}

FCursorReply UImguiInputAdapterDeferred::OnCursorQuery(const FPointerEvent& CursorEvent)
{
	switch (Cursor)
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

void UImguiInputAdapterDeferred::ApplyInput()
{
	if (!GetContext()) return;
	ImGuiIO* IO = &GetContext()->IO;
	
	// copy data 
	IO->InputQueueCharacters = InputQueueCharacters;
	IO->KeyCtrl = KeyCtrl;
	IO->KeyShift = KeyShift;
	IO->KeyAlt = KeyAlt;
	FMemory::Memcpy(IO->KeysDown, KeysDown, sizeof(KeysDown));
	FMemory::Memcpy(IO->MouseDown, MouseDown, sizeof(MouseDown));
	IO->MousePos = MousePos;
	IO->MouseWheel = MouseWheel;

	// clean data
	InputQueueCharacters.clear();
	MouseWheel = 0;
}

void UImguiInputAdapterDeferred::SaveTempData()
{
	Cursor = GetContext()->MouseCursor;
}
