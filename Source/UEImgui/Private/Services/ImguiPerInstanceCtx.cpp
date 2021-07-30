#include "ImguiPerInstanceCtx.h"
#include "imgui_internal.h"
#include "ImguiWrap/ImguiGlobalInputHook.h"
#include "ImguiWrap/ImguiInputAdapter.h"
#include "ImguiWrap/ImguiResourceManager.h"

void UImguiPerInstanceCtx::Initialize(FSubsystemCollectionBase& Collection)
{
	// create global context
	GlobalContext = NewObject<UImguiContext>();

	// create input adapter
	InputAdapter = NewObject<UImguiInputAdapterDeferred>();
	InputAdapter->SetContext(GlobalContext);

	// setup default adapter
	GlobalContext->SetDefaultInputAdapter(InputAdapter);
	
	// add adapter
	FImguiGlobalInputHook::Get()->AddAdapter(InputAdapter);

	// add tick function
	PreSlateTick = FSlateApplication::Get().OnPreTick().AddUObject(this, &UImguiPerInstanceCtx::Tick);
}

void UImguiPerInstanceCtx::Deinitialize()
{
	if (InputAdapter)
	{
		// remove adapter 
		FImguiGlobalInputHook::Get()->RemoveAdapter(InputAdapter);
		InputAdapter = nullptr;
	}
	
	if (GlobalContext)
	{
		// shutdown global context
		GlobalContext->ShutDown();
		GlobalContext = nullptr;
	}

	// remove tick function
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().OnPreTick().Remove(PreSlateTick);
	}
}

void UImguiPerInstanceCtx::Tick(float DeltaTime)
{
	if (!GlobalContext) return;
	if (!GlobalContext->IsInit())
	{
		// find viewport client
		UGameViewportClient* ViewportClient = GetGameInstance()->GetGameViewportClient();
		if (!ViewportClient) return;

		// create proxy
		TSharedPtr<SImguiRenderProxy> Proxy = SNew(SImguiRenderProxy)
		.InContext(GlobalContext)
		.InAdapter(InputAdapter)
		.HSizingRule(EImguiSizingRule::UESize)
		.VSizingRule(EImguiSizingRule::UESize)
		.BlockInput(true)
		.Visibility(EVisibility::HitTestInvisible);
		
		// add to viewport
		ViewportClient->AddViewportWidgetContent(Proxy->AsShared(), INT_MAX);

		// init context
		GlobalContext->Init(Proxy, UImguiResourceManager::Get().GetDefaultFont());
		GlobalContext->EnableDocking(true);
		GlobalContext->EnableViewport(true);
		GlobalContext->EnableDPIScale(true);
		GlobalContext->EnableNoAutoMergeViewport(true);

		// set viewport manually 
		StaticCastSharedPtr<IImguiViewport>(Proxy)->SetupViewport(GlobalContext->GetContext()->Viewports[0]);
		
		return;
	}

	// update main viewport size
	GlobalContext->UpdateSize();
	
	// apply context 
	GlobalContext->ApplyContext();

	// apply input 
	InputAdapter->ApplyInput();
	InputAdapter->SaveTempData();
	
	// begin frame
	GlobalContext->NewFrame(DeltaTime);

	// draw global
	GlobalContext->DrawGlobal();

	// render
	GlobalContext->Render();

	// update viewport 
	GlobalContext->UpdateViewport(InputAdapter);
}

void UImguiPerInstanceCtx::BeginDestroy()
{
	Super::BeginDestroy();
	
	Deinitialize();
}
