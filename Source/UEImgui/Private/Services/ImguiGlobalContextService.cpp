#include "Services/ImguiGlobalContextService.h"
#include "imgui_internal.h"
#include "Framework/Application/IInputProcessor.h"
#include "ImguiWrap/ImguiContext.h"
#include "ImguiWrap/ImguiInputAdapterDeferred.h"
#include "Widgets/SImguiWindow.h"
#include "ImguiWrap/ImguiResourceManager.h"
#include "ImguiWrap/ImguiUEWrap.h"
#include "Widgets/SToolTip.h"
#include "Widgets/Images/SImage.h"

void FImguiGlobalInputHook::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
}

bool FImguiGlobalInputHook::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	// for (UImguiInputAdapter* Adapter : TargetAdapters)
	// {
	// 	Adapter->OnKeyDown(InKeyEvent);
	// }
	return false;
}

bool FImguiGlobalInputHook::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	// for (UImguiInputAdapter* Adapter : TargetAdapters)
	// {
	// 	Adapter->OnKeyUp(InKeyEvent);
	// }
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
	// for (UImguiInputAdapter* Adapter : TargetAdapters)
	// {
	// 	Adapter->OnMouseButtonDown(MouseEvent);
	// }
	return false;
}

bool FImguiGlobalInputHook::HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	// for (UImguiInputAdapter* Adapter : TargetAdapters)
	// {
	// 	Adapter->OnMouseButtonUp(MouseEvent);
	// }
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

UImguiGlobalContextService::UImguiGlobalContextService()
	: GlobalContext(nullptr)
{
}

bool UImguiGlobalContextService::TimeToDraw()
{
	return InputAdapter != nullptr && GlobalContext != nullptr;
}

void UImguiGlobalContextService::AddRenderProxy(TWeakPtr<SImguiWidgetRenderProxy> InRenderProxy)
{
	if (!InRenderProxy.IsValid()) return;
	auto InRenderProxySP = InRenderProxy.Pin();
	InRenderProxySP->SetContext(GlobalContext);
	InRenderProxySP->SetAdapter(InputAdapter);
	AllRenderProxy.AddUnique(InRenderProxy);
}

void UImguiGlobalContextService::Initialize(FSubsystemCollectionBase& Collection)
{
	if (FSlateApplication::IsInitialized())
	{
		GlobalInputHook = MakeShared<FImguiGlobalInputHook>();
		FSlateApplication::Get().OnPreTick().AddUObject(this, &UImguiGlobalContextService::_OnSlatePreTick);
		FSlateApplication::Get().RegisterInputPreProcessor(GlobalInputHook);
	}
}

void UImguiGlobalContextService::Deinitialize()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().OnPreTick().Remove(PreTickHandle);
	}
}

void UImguiGlobalContextService::_OnSlatePreTick(float DeltaTime)
{
	if (!GlobalContext)
	{
		// find global context 
		GlobalContext = UImguiResourceManager::Get().GetGlobalContext();
		if (!GlobalContext) return;

		// create adapter
		if (!InputAdapter) InputAdapter = NewObject<UImguiInputAdapterDeferred>();
		InputAdapter->SetContext(GlobalContext);

		// add to global input hook
		GlobalInputHook->AddAdapter(InputAdapter);

		// begin first frame 
		GlobalContext->ApplyContext();
		ImGui::NewFrame();
		return;
	}
	GlobalContext->ApplyContext();

	// please make sure imgui background context always global context
	check(ImGui::GetCurrentContext() == GlobalContext->GetContext());
	
	// render 
	ImGui::Render();
	
	// dispatch window, create UE window for all shown imgui window 
	_DispatchWindows();

	// apply current frame input 
	// imgui message listen scope is EndFrame()->BeginFrame(), but we want to capture Global imgui draw
	// so we use deferred input adapter here
	// by this, global imgui always have one frame delay, but it not a problem 
	InputAdapter->ApplyInput();
	InputAdapter->SaveTempData();

	// begin a new frame to capture global imgui draw 
	ImGui::NewFrame();
	
	// draw global windows 
	_DrawGlobalImguiWnds();
}

void UImguiGlobalContextService::_DrawGlobalImguiWnds()
{
	for (int32 i = 0; i < AllDrawCallBack.Num();)
	{
		auto& Delegate = AllDrawCallBack[i];
		if (Delegate.IsBound())
		{
			bool bIsWndAlive = Delegate.Execute();
			if (!bIsWndAlive)
			{
				AllDrawCallBack.RemoveAtSwap(i);				
			}
			else
			{
				++i;
			}
		}
		else
		{
			AllDrawCallBack.RemoveAtSwap(i);
		}
	}
}

void UImguiGlobalContextService::_DispatchWindows()
{
	ImGuiContext* Ctx = GlobalContext->GetContext();

	// clean up render proxy 
	_CleanUpRenderProxy();
	
	// child windows exclude Menu, ToolTip and Popup  
	TArray<ImGuiWindow*, TInlineAllocator<100>> ChildWndArr;
	// top side windows 
	TArray<ImGuiWindow*, TInlineAllocator<100>> TopSideWndArr;

	// collect top side windows and child windows  
	for (int i = 0; i < Ctx->Windows.size(); ++i)
	{
		ImGuiWindow* Wnd = Ctx->Windows[i];
		if (!Wnd) continue;
		if (!Wnd->Active && !Wnd->WasActive)
		{
			TSharedPtr<SImguiWindow>* FoundWnd = ImguiUnrealWindows.Find(Wnd->ID);
			if (!FoundWnd) continue;
			if (Wnd->Flags & ImGuiWindowFlags_Modal)
			{
				FoundWnd->Get()->RequestDestroyWindow();
			}
			else
			{
				FoundWnd->Get()->HideWindow();
			}
			continue;
		}
		
		if (_IsMenu(Wnd))
		{
			if (!_IsInnerChild(Wnd))
			{
				TopSideWndArr.Add(Wnd);
				continue;
			}
		}
		if (_IsToolTip(Wnd))
		{
			if (!_IsInnerChild(Wnd))
			{
				TopSideWndArr.Add(Wnd);
				continue;
			}
		}
		if (_IsPopup(Wnd))
		{
			if (!_IsInnerChild(Wnd))
			{
				TopSideWndArr.Add(Wnd);
				continue;
			}
		}
		if (Wnd->ParentWindow == nullptr)
		{
			TopSideWndArr.Add(Wnd);
			continue;
		}
		
		ChildWndArr.Add(Wnd);
	}

	// build windows 
	for (ImGuiWindow* Wnd : TopSideWndArr)
	{
		TWeakPtr<SImguiWidgetRenderProxy> Proxy = _FineRenderProxy(Wnd);
		if (Proxy.IsValid())
		{
			TSharedPtr<SImguiWidgetRenderProxy> ProxySP = Proxy.Pin();
			ProxySP->SetTopWnd(Wnd->ID);
			auto& IDArr = ProxySP->GetWndID();
			IDArr.Reset();
			IDArr.Add(Wnd->ID);
			for (ImGuiWindow* Child : ChildWndArr)
			{
				if (_IsClosestParent(Child, Wnd))
				{
					IDArr.Add(Child->ID);
				}
			}
		}
		else
		{
			// find window 
			TSharedPtr<SImguiWindow> ImguiWindow = _FindUnrealWindow(Wnd);

			// set window info
			ImguiWindow->MoveWindowTo({ Wnd->Pos.x, Wnd->Pos.y });
			ImguiWindow->SetTitle(FText::FromString(Wnd->Name));
			ImguiWindow->Resize({ Wnd->Size.x, Wnd->Size.y });
			
			// bind window id
			ImguiWindow->SetTopWnd(Wnd->ID);
			auto& IDArr = ImguiWindow->GetWndID();
			IDArr.Reset();
			IDArr.Add(Wnd->ID);
			for (ImGuiWindow* Child : ChildWndArr)
			{
				if (_IsClosestParent(Child, Wnd))
				{
					IDArr.Add(Child->ID);
				}
			}
		}
	}
}

bool UImguiGlobalContextService::_IsParent(ImGuiWindow* InChild, ImGuiWindow* InParent)
{
	while (InChild->ParentWindow)
	{
		if (InChild->ParentWindow == InParent) return true;
		InChild = InChild->ParentWindow;
	}
	return false;
}

bool UImguiGlobalContextService::_IsClosestParent(ImGuiWindow* InChild, ImGuiWindow* InParent)
{
	while (InChild->ParentWindow)
	{
		if (InChild->ParentWindow == InParent) return true;
		if (_IsPopup(InChild->ParentWindow) || _IsMenu(InChild->ParentWindow) || _IsToolTip(InChild->ParentWindow)) return false;
		InChild = InChild->ParentWindow;
	}
	return false;
}

bool UImguiGlobalContextService::_IsToolTip(ImGuiWindow* InWnd)
{
	return FCStringAnsi::Strncmp(InWnd->Name, "##ToolTip", 9) == 0;
}

bool UImguiGlobalContextService::_IsMenu(ImGuiWindow* InWnd)
{
	return FCStringAnsi::Strncmp(InWnd->Name, "##Menu", 6) == 0;
}

bool UImguiGlobalContextService::_IsPopup(ImGuiWindow* InWnd)
{
	return
		FCStringAnsi::Strncmp(InWnd->Name, "##Popup", 7) == 0 ||
		FCStringAnsi::Strncmp(InWnd->Name, "##Combo", 7) == 0;
}

bool UImguiGlobalContextService::_IsInnerChild(ImGuiWindow* InWnd)
{
	char* Pos = FCStringAnsi::Strchr(InWnd->Name, '/');
	if (!Pos) return false;
	Pos = FCStringAnsi::Strchr(Pos, '_');
	if (!Pos) return false;
	++Pos;
	while (*Pos)
	{
		if (!(*Pos >= '0' && *Pos <= '9') && !(*Pos >= 'A' && *Pos <= 'F')) return false;
		++Pos;
	}
	return true;
}

bool UImguiGlobalContextService::_IsChildOfPopupWnd(ImGuiWindow* InWnd)
{
	return InWnd->ParentWindow ?
		_IsMenu(InWnd->ParentWindow) || _IsToolTip(InWnd->ParentWindow) || _IsPopup(InWnd->ParentWindow)
		: false;
}

TWeakPtr<SImguiWidgetRenderProxy> UImguiGlobalContextService::_FineRenderProxy(ImGuiWindow* InWindow)
{
	for (TWeakPtr<SImguiWidgetRenderProxy> Proxy : AllRenderProxy)
	{
		if (!Proxy.IsValid()) continue;;
		TSharedPtr<SImguiWidgetRenderProxy> ProxySp = Proxy.Pin();
		if (ProxySp->GetTopWnd() == InWindow->ID)
		{
			return Proxy;
		}
	}
	return nullptr;
}

void UImguiGlobalContextService::_CleanUpRenderProxy()
{
	for (int32 i = 0; i < AllRenderProxy.Num();)
	{
		if (!AllRenderProxy[i].IsValid())
		{
			AllRenderProxy.RemoveAt(i);
		}
		else
		{
			++i;
		}
	}
}

TSharedPtr<SImguiWindow> UImguiGlobalContextService::_FindUnrealWindow(ImGuiWindow* InWindow)
{
	TSharedPtr<SImguiWindow>* FoundWnd = ImguiUnrealWindows.Find(InWindow->ID);
	TSharedPtr<SImguiWindow> ImguiWindow;

	// create a new window 
	if (!FoundWnd)
	{
		bool bIsMenu = _IsMenu(InWindow);
		bool bIsToolTip = _IsToolTip(InWindow);
		bool bIsPopUp = _IsPopup(InWindow);
		SAssignNew(ImguiWindow, SImguiWindow)
			.Context(GlobalContext)
			.IsMenu(bIsMenu)
			.IsToolTip(bIsToolTip)
			.IsPopup(bIsPopUp);
		
		ImguiUnrealWindows.Add(InWindow->ID, ImguiWindow);
		ImguiWindow->SetOnWindowClosed(FOnWindowClosed::CreateLambda(
			[&](const TSharedRef<SWindow>& InWnd)
			{
				auto Key = ImguiUnrealWindows.FindKey(StaticCastSharedRef<SImguiWindow>(InWnd));
				ImGuiWindow* Wnd = (ImGuiWindow*)GlobalContext->GetContext()->WindowsById.GetVoidPtr(*Key);
				Wnd->WasActive = false;
				ImguiUnrealWindows.Remove(*Key);
			}));

		if (InWindow->Flags & ImGuiWindowFlags_Modal)
		{
			FDisplayMetrics DisplayMetrics;
			FDisplayMetrics::RebuildDisplayMetrics(DisplayMetrics);
			InWindow->Pos.x = (DisplayMetrics.PrimaryDisplayWidth - InWindow->Size.x) / 2;
			InWindow->Pos.y = (DisplayMetrics.PrimaryDisplayHeight - InWindow->Size.y) / 2;
			FSlateApplication::Get().AddModalWindow(ImguiWindow.ToSharedRef(), nullptr);
		}
		else
		{
			FSlateApplication::Get().AddWindow(ImguiWindow.ToSharedRef());
		}
	}
	else
	{
		ImguiWindow = *FoundWnd;
		ImguiWindow->ShowWindow();
	}

	// set up adapter 
	ImguiWindow->SetAdapter(InputAdapter);

	return ImguiWindow;
}
