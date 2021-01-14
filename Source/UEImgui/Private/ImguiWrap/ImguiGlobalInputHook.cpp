#include "ImguiWrap/ImguiGlobalInputHook.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "ImguiWrap/ImguiContext.h"
#include "ImguiWrap/ImguiInputAdapter.h"

void FImguiGlobalInputHook::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
}

bool FImguiGlobalInputHook::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	// for (UImguiInputAdapter* Adapter : TargetAdapters)
	// {
	// 	if (!Adapter->GetContext()) continue;
	// 	if (Adapter->GetContext()->GetIO()->WantCaptureMouse)
	// 	{
	// 		Adapter->OnKeyDown(InKeyEvent);
	// 	}
	// }
	return false;
}

bool FImguiGlobalInputHook::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	for (UImguiInputAdapter* Adapter : TargetAdapters)
	{
		if (!Adapter->GetContext()) continue;
		// up event never block 
		// if (Adapter->GetContext()->GetIO()->WantCaptureKeyboard)
		{
			Adapter->OnKeyUp(InKeyEvent);
		}
	}
	return false;
}

bool FImguiGlobalInputHook::HandleAnalogInputEvent(FSlateApplication& SlateApp,
	const FAnalogInputEvent& InAnalogInputEvent)
{
	// for (UImguiInputAdapter* Adapter : TargetAdapters)
	// {
	// 	
	// }
	return false;
}

bool FImguiGlobalInputHook::HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	for (UImguiInputAdapter* Adapter : TargetAdapters)
	{
		if (!Adapter->GetContext()) continue;
		if (Adapter->GetContext()->GetIO()->WantCaptureMouse)
		{
			Adapter->OnMouseMove(FVector2D::ZeroVector, MouseEvent);
		}
	}
	return false;
}

bool FImguiGlobalInputHook::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	for (UImguiInputAdapter* Adapter : TargetAdapters)
	{
		if (!Adapter->GetContext()) continue;
		if (Adapter->GetContext()->GetIO()->WantCaptureMouse)
		{
			Adapter->OnMouseButtonDown(MouseEvent);
		}
	}
	return false;
}

bool FImguiGlobalInputHook::HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	for (UImguiInputAdapter* Adapter : TargetAdapters)
	{
		if (!Adapter->GetContext()) continue;
		// up event never block 
		// if (Adapter->GetContext()->GetIO()->WantCaptureMouse)
		{
			Adapter->OnMouseButtonUp(MouseEvent);
		}
	}
	return false;
}

bool FImguiGlobalInputHook::HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp,
	const FPointerEvent& MouseEvent)
{
	// for (UImguiInputAdapter* Adapter : TargetAdapters)
	// {
	// 	Adapter->OnMouseButtonDoubleClick(MouseEvent);
	// }
	return false;
}

bool FImguiGlobalInputHook::HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp,
	const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent)
{
	// for (UImguiInputAdapter* Adapter : TargetAdapters)
	// {
	// 	Adapter->OnMouseWheel(InWheelEvent);
	// }
	return false;
}

bool FImguiGlobalInputHook::HandleMotionDetectedEvent(FSlateApplication& SlateApp, const FMotionEvent& MotionEvent)
{
	// for (UImguiInputAdapter* Adapter : TargetAdapters)
	// {
	// 	
	// }
	return false;
}

void FImguiGlobalInputHook::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObjects(TargetAdapters);
}

TSharedPtr<FImguiGlobalInputHook> FImguiGlobalInputHook::Get()
{
	static TSharedPtr<FImguiGlobalInputHook> Instance;
	if (!FSlateApplication::IsInitialized()) return nullptr;
	if (!Instance.IsValid())
	{
		Instance = MakeShared<FImguiGlobalInputHook>();
		FSlateApplication::Get().RegisterInputPreProcessor(Instance);
	}
	return Instance;
}

void FImguiGlobalInputHook::AddAdapter(UImguiInputAdapter* InInputAdapter)
{
	check(InInputAdapter);
	TargetAdapters.AddUnique(InInputAdapter);
}

void FImguiGlobalInputHook::RemoveAdapter(UImguiInputAdapter* InInputAdapter)
{
	check(InInputAdapter);
	TargetAdapters.Remove(InInputAdapter);
}