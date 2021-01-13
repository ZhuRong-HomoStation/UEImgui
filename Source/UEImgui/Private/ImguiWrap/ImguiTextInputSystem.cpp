#include "ImguiTextInputSystem.h"
#include "imgui_internal.h"
#include "ImguiWrap/ImguiInputAdapter.h"
#include "ImguiWrap/ImguiUEWrap.h"

void FImguiTextInputSystem::Enable()
{
	if (bIsEnable) return;
	FSlateApplication::Get().GetTextInputMethodSystem()->RegisterContext(AsShared());
	FSlateApplication::Get().GetTextInputMethodSystem()->ActivateContext(AsShared());
	bIsEnable = true;
}

void FImguiTextInputSystem::Disable()
{
	if (!bIsEnable) return;
	FSlateApplication::Get().GetTextInputMethodSystem()->DeactivateContext(AsShared());
	FSlateApplication::Get().GetTextInputMethodSystem()->UnregisterContext(AsShared());
	bIsEnable = false;
}

bool FImguiTextInputSystem::IsComposing()
{
	return false;
}

void FImguiTextInputSystem::BeginComposition()
{
}

void FImguiTextInputSystem::UpdateCompositionRange(const int32 InBeginIndex, const uint32 InLength)
{
}

void FImguiTextInputSystem::EndComposition()
{
}

bool FImguiTextInputSystem::IsReadOnly()
{
	return false;
}

uint32 FImguiTextInputSystem::GetTextLength()
{
	return 1;
}

void FImguiTextInputSystem::GetSelectionRange(uint32& OutBeginIndex, uint32& OutLength, ECaretPosition& OutCaretPosition)
{
	OutBeginIndex = 0;
	OutLength = 0;
}

void FImguiTextInputSystem::SetSelectionRange(const uint32 InBeginIndex, const uint32 InLength, const ECaretPosition InCaretPosition)
{
}

void FImguiTextInputSystem::GetTextInRange(const uint32 InBeginIndex, const uint32 InLength, FString& OutString)
{
	OutString.Append(TEXT(" "), InLength);
}

void FImguiTextInputSystem::SetTextInRange(const uint32 InBeginIndex, const uint32 InLength, const FString& InString)
{
	if (InString.IsEmpty() || (InString.Len() == 1 && InString[0] == TEXT(' ')) || CurrentInputAdapter == nullptr) return;
	CurrentInputAdapter->AddInputIME(InString);
}

int32 FImguiTextInputSystem::GetCharacterIndexFromPoint(const FVector2D& InPoint)
{
	return 0;
}

bool FImguiTextInputSystem::GetTextBounds(
	const uint32 InBeginIndex,
	const uint32 InLength,
	FVector2D& OutPosition,
	FVector2D& OutSize)
{
	OutPosition = InputPos;
	OutSize = FVector2D::ZeroVector;
	return false;
}

void FImguiTextInputSystem::GetScreenBounds(FVector2D& OutPosition, FVector2D& OutSize)
{
	OutPosition = InputPos;
	OutSize = FVector2D::ZeroVector;
}

TSharedPtr<FGenericWindow> FImguiTextInputSystem::GetWindow()
{
	return CurrentWindow.IsValid() ? CurrentWindow.Pin()->GetNativeWindow() : nullptr;
}
