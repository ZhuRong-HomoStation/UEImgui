#include "ImguiWrap/ImguiContext.h"
#include "imgui.h"
#include "ImguiTextInputSystem.h"
#include "imgui_internal.h"
#include "GenericPlatform/ITextInputMethodSystem.h"
#include "ImguiWrap/ImguiInputAdapter.h"
#include "ImguiWrap/ImguiUEWrap.h"
#include "Widgets/SImguiWindow.h"
#include "Window/ImguiWindowWrapper.h"

void UImguiContext::BeginDestroy()
{
	Super::BeginDestroy();
	
	ShutDown();
}

void UImguiContext::Init(TSharedPtr<IImguiViewport> InMainViewPort, ImFontAtlas* InDefaultFontAtlas)
{
	// create context 
	Context = ImGui::CreateContext(InDefaultFontAtlas);

	// init font
	if (!InDefaultFontAtlas)
	{
		_SetUpDefaultFont();
	}

	// set up context 
	_SetupImguiContext();

	// set up key map 
	UImguiInputAdapter::CopyUnrealKeyMap(GetIO());

	// setup main view port
	MainViewPort = InMainViewPort;
	auto ImMainViewPort = Context->Viewports[0];
	ImMainViewPort->PlatformHandle = ImMainViewPort->PlatformHandleRaw = InMainViewPort.Get();

	// setup main client size
	auto MainViewPortSize = InMainViewPort->GetSize();
	GetIO()->DisplaySize = *(ImVec2*)&MainViewPortSize;

	// add to search map
	ImViewportToUE.Add(ImMainViewPort, MainViewPort);
}

void UImguiContext::UpdateSize()
{
	if (!MainViewPort.IsValid())
	{
		return;
	}

	// setup main client size
	auto MainViewPortSize = MainViewPort->GetSize();
	GetIO()->DisplaySize = *(ImVec2*)&MainViewPortSize;
}

void UImguiContext::ShutDown()
{
	// destroy context 
	if (Context) ImGui::DestroyContext(Context);

	// clean reference
	Context = nullptr;

	// clean dispatched viewport
	for (TSharedPtr<IImguiViewport>& Viewport : AllDispatchedViewport)
	{
		if (FSlateApplication::IsInitialized())
		{
			FSlateApplicationBase::Get().RequestDestroyWindow(StaticCastSharedPtr<SImguiWindow>(Viewport).ToSharedRef());
		}
	}

	MainViewPort.Reset();
	AllDrawCallBack.Reset();
	AllRenderProxy.Reset();
	AllDispatchedViewport.Reset();
	ImViewportToUE.Reset();
}

void UImguiContext::EnableDocking(bool bInEnable)
{
	if (bInEnable)
	{
		GetIO()->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	}
	else
	{
		if (GetIO()->ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			GetIO()->ConfigFlags -= ImGuiConfigFlags_DockingEnable;
		}
	}
}

bool UImguiContext::EnableDocking()
{
	return GetIO()->ConfigFlags & ImGuiConfigFlags_DockingEnable;
}

void UImguiContext::EnableViewport(bool bInEnable)
{
	if (bInEnable)
	{
		GetIO()->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	}
	else
	{
		if (GetIO()->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GetIO()->ConfigFlags -= ImGuiConfigFlags_ViewportsEnable;
		}
	}
}

bool UImguiContext::EnableViewport()
{
	return GetIO()->ConfigFlags & ImGuiConfigFlags_ViewportsEnable;
}

void UImguiContext::EnableNoAutoMergeViewport(bool bInIsEnable)
{
	GetIO()->ConfigViewportsNoAutoMerge = bInIsEnable;
}

bool UImguiContext::EnableNoAutoMergeViewport()
{
	return GetIO()->ConfigViewportsNoAutoMerge;
}

void UImguiContext::AddRenderProxy(TWeakPtr<IImguiViewport> InRenderProxy)
{
	check(InRenderProxy.IsValid() && InRenderProxy.Pin()->IsPersist());
	InRenderProxy.Pin()->SetupContext(this);
	InRenderProxy.Pin()->SetupInputAdapter(DefaultAdapter);
	AllRenderProxy.Add(InRenderProxy);
}

void UImguiContext::RemoveRenderProxy(TWeakPtr<IImguiViewport> InRenderProxy)
{
	AllRenderProxy.Remove(InRenderProxy);
}

ImGuiIO* UImguiContext::GetIO() const
{
	return &Context->IO;
}

ImGuiStyle* UImguiContext::GetStyle() const
{
	return &Context->Style;
}

void UImguiContext::ApplyContext()
{
	ImGui::SetCurrentContext(Context);
	FImguiWindowWrapper::CurContext = this;
}

void UImguiContext::NewFrame(float DeltaTime)
{
	ImGui::GetIO().DeltaTime = DeltaTime;
	ImGui::NewFrame();
}

void UImguiContext::DrawGlobal()
{
	for (int32 i = 0; i < AllDrawCallBack.Num(); ++i)
	{
		auto& CurCallBack = AllDrawCallBack[i];
		
		if (!CurCallBack.IsBound()) continue;
		bool bNeedClose = !CurCallBack.Execute();
		if (bNeedClose)
		{
			CurCallBack.Unbind();
		}
	}
}

void UImguiContext::Render()
{
	ImGui::Render();
}

void UImguiContext::UpdateViewport(UImguiInputAdapter* InAdapter)
{
	FImguiWindowWrapper::CurContext = this;
	FImguiWindowWrapper::CurInputAdapter = InAdapter;
	ImGui::UpdatePlatformWindows();
}

void UImguiContext::_CreateWindow(ImGuiViewport* viewport, UImguiInputAdapter* InInputAdapter)
{
	// use proxy
	{
		auto Proxy = _DispatchProxy(viewport, InInputAdapter);
		if (Proxy.IsValid())
		{
			// setup platform data
			viewport->PlatformHandle = viewport->PlatformHandleRaw = Proxy.Pin().Get();
			viewport->PlatformUserData = InInputAdapter;
			
			// add to array 
			ImViewportToUE.Add(viewport, Proxy);
			return;
		}
	}

	// create a real window
	bool bIsToolTip = viewport->Flags & ImGuiViewportFlags_TopMost;
	bool bIsPopUp = viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon;
	bool bTakeFocusWhenShow = !(viewport->Flags & ImGuiViewportFlags_NoFocusOnAppearing);
	TSharedPtr<SImguiWindow> UEWnd = SNew(SImguiWindow)
		.Context(this)
		.Adapter(InInputAdapter)
		.Viewport(viewport)
		.IsToolTip(bIsToolTip)
		.IsPopup(bIsPopUp)
		.TakeFocusWhenShow(bTakeFocusWhenShow);
	UEWnd->SetContext(this);
	UEWnd->SetAdapter(InInputAdapter);
	StaticCastSharedPtr<IImguiViewport>(UEWnd)->SetupViewport(viewport);

	// add to array
	AllDispatchedViewport.Add(UEWnd);
	ImViewportToUE.Add(viewport, UEWnd);

	// setup platform data
	viewport->PlatformHandle = viewport->PlatformHandleRaw = UEWnd.Get();
	viewport->PlatformUserData = InInputAdapter;
}

void UImguiContext::_DestroyWindow(ImGuiViewport* viewport)
{
	// find UE widget
	auto UEWidget = ImViewportToUE.Find(viewport);
	if (!UEWidget)
	{
		// disable platform data 
		viewport->PlatformUserData = nullptr;
		viewport->PlatformHandle = nullptr;
		return;
	}
	
	// process dispatched window case 
	if (UEWidget->IsValid() && !UEWidget->Pin()->IsPersist())
	{
		auto PinnedUEWidget = UEWidget->Pin();
		if (FSlateApplication::IsInitialized())
		{
			FSlateApplicationBase::Get().RequestDestroyWindow(StaticCastSharedPtr<SImguiWindow>(PinnedUEWidget).ToSharedRef());
		}
		AllDispatchedViewport.Remove(PinnedUEWidget);
	}
	
	// remove from map 
	ImViewportToUE.Remove(viewport);

	// disable platform data 
	viewport->PlatformUserData = nullptr;
	viewport->PlatformHandle = nullptr;
}

void UImguiContext::_ShowWindow(ImGuiViewport* viewport)
{
	// find UE widget
	auto UEWidget = _SafeFindViewport(viewport, false);

	// find parent 
	auto ParentViewPort = ImGui::FindViewportByID(viewport->ParentViewportId);
	auto ParentWidget = _SafeFindViewport(ParentViewPort);

	// show it!!!
	UEWidget->Show(ParentWidget->GetWindow());
}

void UImguiContext::_UpdateWindow(ImGuiViewport* viewport)
{
	_SafeFindViewport(viewport)->Update();
}

ImVec2 UImguiContext::_GetWindowPos(ImGuiViewport* viewport)
{
	auto Pos = _SafeFindViewport(viewport)->GetPos();
	return *(ImVec2*)&Pos;
}

void UImguiContext::_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
{
	_SafeFindViewport(viewport)->SetPos(*(FVector2D*)&pos);
}

ImVec2 UImguiContext::_GetWindowSize(ImGuiViewport* viewport)
{
	auto Size = _SafeFindViewport(viewport)->GetSize();
	return *(ImVec2*)&Size;
}

void UImguiContext::_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
{
	_SafeFindViewport(viewport)->SetSize(*(FVector2D*)&size);
}

void UImguiContext::_SetWindowFocus(ImGuiViewport* viewport)
{
	_SafeFindViewport(viewport)->SetFocus();
}

bool UImguiContext::_GetWindowFocus(ImGuiViewport* viewport)
{
	return _SafeFindViewport(viewport)->GetFocus();
}

bool UImguiContext::_GetWindowMinimized(ImGuiViewport* viewport)
{
	return _SafeFindViewport(viewport)->GetMinimized();
}

void UImguiContext::_SetWindowTitle(ImGuiViewport* viewport, const char* title)
{
	_SafeFindViewport(viewport)->SetTitle(title);
}

void UImguiContext::_SetWindowAlpha(ImGuiViewport* viewport, float alpha)
{
	_SafeFindViewport(viewport)->SetAlpha(alpha);
}

float UImguiContext::_GetWindowDpiScale(ImGuiViewport* viewport)
{
	return _SafeFindViewport(viewport)->GetDpiScale();
}

void UImguiContext::_OnChangedViewport(ImGuiViewport* viewport)
{
	if (!ImViewportToUE.Contains(viewport)) return;
	_SafeFindViewport(viewport)->OnChangeViewport();
}

void UImguiContext::_SetImeInputPos(ImGuiViewport* viewport, ImVec2 pos)
{
	if (!ImViewportToUE.Contains(viewport)) return;
	TSharedPtr<IImguiViewport> Viewport = _SafeFindViewport(viewport);

	if (pos.x == -1 && pos.y == -1)
	{
		// disable IME 
		auto IME = FImguiTextInputSystem::GetRef();
		IME->CurrentWindow = nullptr;
		IME->InputPos = FVector2D::ZeroVector;
		IME->CurrentInputAdapter = nullptr;
		IME->Disable();
	}
	else
	{
		// enable IME 
		auto IME = FImguiTextInputSystem::GetRef();
		IME->CurrentWindow = Viewport->GetWindow();
		IME->InputPos.X = pos.x;
		IME->InputPos.Y = pos.y + Context->IO.Fonts->Fonts[0]->FontSize;
		IME->CurrentInputAdapter = DefaultAdapter;
		IME->Enable();
	}
}

void UImguiContext::_SetupImguiContext()
{
	// enable keyboard control
	// GetIO()->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	// GetIO()->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	// enable transparent 
	GetIO()->ConfigDockingTransparentPayload = true;
	
	// set backend flags
	GetIO()->BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	GetIO()->BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
	GetIO()->BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
	GetIO()->BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
	GetIO()->BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
	GetIO()->BackendPlatformName = "Unreal Backend";
	GetIO()->BackendPlatformName = "Unreal Widget";

	// set up style
	ImGui::StyleColorConfig(&GetContext()->Style);

	// disable .ini by default
	GetIO()->IniFilename = nullptr;

	// setup platform interface
	FImguiWindowWrapper::SetupPlatformInterface(&Context->PlatformIO);

	// setup monitor
	FDisplayMetrics DisplayMetrics;
	FDisplayMetrics::RebuildDisplayMetrics(DisplayMetrics);
	
	// no monitor when rdp
	if (FPlatformMisc::IsRemoteSession())
	{
		ImGuiPlatformMonitor Monitor;
		Monitor.DpiScale = 1.f;
		
		Monitor.MainPos.x = DisplayMetrics.VirtualDisplayRect.Left;
		Monitor.MainPos.y = DisplayMetrics.VirtualDisplayRect.Top;
		Monitor.MainSize.x = DisplayMetrics.VirtualDisplayRect.Right - DisplayMetrics.VirtualDisplayRect.Left;
		Monitor.MainSize.y = DisplayMetrics.VirtualDisplayRect.Bottom - DisplayMetrics.VirtualDisplayRect.Top;

		Monitor.WorkPos.x = DisplayMetrics.PrimaryDisplayWorkAreaRect.Left;
		Monitor.WorkPos.y = DisplayMetrics.PrimaryDisplayWorkAreaRect.Top;
		Monitor.WorkSize.x = DisplayMetrics.PrimaryDisplayWorkAreaRect.Right - DisplayMetrics.PrimaryDisplayWorkAreaRect.Left;
		Monitor.WorkSize.y = DisplayMetrics.PrimaryDisplayWorkAreaRect.Bottom - DisplayMetrics.PrimaryDisplayWorkAreaRect.Top;
		Context->PlatformIO.Monitors.push_back(Monitor);
	}
	
	for (FMonitorInfo& Info : DisplayMetrics.MonitorInfo)
	{
		ImGuiPlatformMonitor Monitor;
		Monitor.DpiScale = 1.f;

		Monitor.MainPos.x = Info.DisplayRect.Left;
		Monitor.MainPos.y = Info.DisplayRect.Top;
		Monitor.MainSize.x = Info.DisplayRect.Right - Info.DisplayRect.Left;
		Monitor.MainSize.y = Info.DisplayRect.Bottom - Info.DisplayRect.Top;

		Monitor.WorkPos.x = Info.WorkArea.Left;
		Monitor.WorkPos.y = Info.WorkArea.Top;
		Monitor.WorkSize.x = Info.WorkArea.Right - Info.WorkArea.Left;
		Monitor.WorkSize.y = Info.WorkArea.Bottom - Info.WorkArea.Top;
		Context->PlatformIO.Monitors.push_back(Monitor);
	}
}

void UImguiContext::_SetUpDefaultFont()
{
	if (!GetIO()->Fonts->IsBuilt())
	{
		ImFontConfig FontConfig = {};
		FontConfig.SizePixels = FMath::RoundFromZero(13.f);
		GetIO()->Fonts->AddFontDefault(&FontConfig);
		
		unsigned char* Pixels;
		int Width, Height, Bpp;
		GetIO()->Fonts->GetTexDataAsRGBA32(&Pixels, &Width, &Height, &Bpp);
	}
}

TSharedPtr<IImguiViewport> UImguiContext::_SafeFindViewport(ImGuiViewport* InViewport, bool bNeedShow)
{
	// early out
	if (!InViewport) return nullptr;
	
	// find weak reference 
	auto UEWidget = ImViewportToUE[InViewport];

	// return it directly 
	if (UEWidget.IsValid()) return UEWidget.Pin();

	// create
	_CreateWindow(InViewport, (UImguiInputAdapter*)InViewport->PlatformUserData);

	// get shared ptr
	auto PinedWidget = ImViewportToUE[InViewport].Pin();
	
	// show it
	if (bNeedShow)
	{
		auto ParentViewPort = ImGui::FindViewportByID(InViewport->ParentViewportId);
		auto ParentWidget = _SafeFindViewport(ParentViewPort, bNeedShow);
		PinedWidget->Show(ParentWidget->GetWindow());
	}

	return PinedWidget;
}

TWeakPtr<IImguiViewport> UImguiContext::_DispatchProxy(ImGuiViewport* InViewport, UImguiInputAdapter* InInputAdapter)
{
	for (int32 i = 0; i < AllRenderProxy.Num();)
	{
		auto Proxy = AllRenderProxy[i];

		// check if valid 
		if (!Proxy.IsValid())
		{
			AllRenderProxy.RemoveAtSwap(i);
			continue;
		}

		// find window
		auto PinedProxy = Proxy.Pin();
		check(PinedProxy->IsPersist());
		ImGuiWindow* Window = ImGui::FindWindowByID(PinedProxy->GetPersistWindowID());
		if (Window && Window->Viewport == InViewport)
		{
			PinedProxy->SetupViewport(InViewport);
			PinedProxy->SetupInputAdapter(InInputAdapter);
			return PinedProxy;
		}
		
		++i;
	}

	return nullptr;
}
