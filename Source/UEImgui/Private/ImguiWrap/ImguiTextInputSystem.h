#pragma once
#include "CoreMinimal.h"
#include "GenericPlatform/ITextInputMethodSystem.h"

struct ImGuiContext;
struct ImGuiInputTextState;

class FImguiTextInputSystem : public ITextInputMethodContext
{
public:
	static TSharedRef<FImguiTextInputSystem> GetRef()
	{
		static TSharedRef<FImguiTextInputSystem> StaticInstance = MakeShared<FImguiTextInputSystem>();
		return StaticInstance;
	}

	static FImguiTextInputSystem* Get()
	{
		return &GetRef().Get();
	}
protected:
	// ~Begin ITextInputMethodContext Interface

	// no composing 
	virtual bool IsComposing() override;
	virtual void BeginComposition() override;
	virtual void UpdateCompositionRange(const int32 InBeginIndex, const uint32 InLength) override;
	virtual void EndComposition() override;

	virtual bool IsReadOnly() override;
	virtual uint32 GetTextLength() override;
	virtual void GetSelectionRange(uint32& OutBeginIndex, uint32& OutLength, ECaretPosition& OutCaretPosition) override;
	virtual void SetSelectionRange(const uint32 InBeginIndex, const uint32 InLength, const ECaretPosition InCaretPosition) override;
	virtual void GetTextInRange(const uint32 InBeginIndex, const uint32 InLength, FString& OutString) override;
	virtual void SetTextInRange(const uint32 InBeginIndex, const uint32 InLength, const FString& InString) override;
	virtual int32 GetCharacterIndexFromPoint(const FVector2D& InPoint) override;
	virtual bool GetTextBounds(const uint32 InBeginIndex, const uint32 InLength, FVector2D& OutPosition, FVector2D& OutSize) override;
	virtual void GetScreenBounds(FVector2D& OutPosition, FVector2D& OutSize) override;
	virtual TSharedPtr<FGenericWindow> GetWindow() override;
	// ~End ITextInputMethodContext Interface 
public:
	FVector2D				InputPos;
	TWeakPtr<SWindow>		CurrentWindow;
};
