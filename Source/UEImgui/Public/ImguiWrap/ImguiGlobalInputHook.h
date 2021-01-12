#pragma once
#include "CoreMinimal.h"
#include "Framework/Application/IInputProcessor.h"

class UImguiInputAdapter;

// hook global input when we need 
class FImguiGlobalInputHook : public IInputProcessor, public FGCObject
{
public:
	static TSharedPtr<FImguiGlobalInputHook> Get();
	
	void AddAdapter(UImguiInputAdapter* InInputAdapter);
	void RemoveAdapter(UImguiInputAdapter* InInputAdapter);
protected:
	// ~Begin IInputProcessor API 
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override;
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent) override;
	virtual bool HandleMotionDetectedEvent(FSlateApplication& SlateApp, const FMotionEvent& MotionEvent) override;
	// ~End IInputProcessor API

	// ~Begin FGCObject API 
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	// ~End FGCObject API
private:
	TArray<UImguiInputAdapter*>			TargetAdapters;
};
