#include "ImguiPerInstanceCtx.h"
#include "imgui_internal.h"
#include "ImguiWrap/ImguiInputAdapter.h"
#include "ImguiWrap/ImguiResourceManager.h"

void UImguiPerInstanceCtx::Initialize(FSubsystemCollectionBase& Collection)
{
	// create global context
	GlobalContext = NewObject<UImguiContext>();

	// create input adapter
	InputAdapter = NewObject<UImguiInputAdapter>();
	InputAdapter->SetContext(GlobalContext);
}

void UImguiPerInstanceCtx::Deinitialize()
{
	// shutdown global context
	GlobalContext->ShutDown();
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
		TSharedPtr<SImguiWidgetRenderProxy> Proxy = SNew(SImguiWidgetRenderProxy)
		.InContext(GlobalContext)
		.InAdapter(InputAdapter)
		.HSizingRule(EImguiSizingRule::UESize)
		.VSizingRule(EImguiSizingRule::UESize)
		.BlockInput(false);
		
		// add to viewport
		ViewportClient->AddViewportWidgetContent(Proxy->AsShared(), INT_MAX);

		// init context
		GlobalContext->Init(Proxy, UImguiResourceManager::Get().GetDefaultFont());

		// set viewport manually 
		StaticCastSharedPtr<IImguiViewport>(Proxy)->SetupViewport(GlobalContext->GetContext()->Viewports[0]);
		
		return;
	}

	// update main viewport size
	GlobalContext->UpdateSize();

	// set up context info
	GlobalContext->GetIO()->DeltaTime = DeltaTime;
	
	// apply context 
	GlobalContext->ApplyContext();

	// begin frame
	GlobalContext->NewFrame();

	// draw global
	GlobalContext->DrawGlobal();

	// render
	GlobalContext->Render();

	// update viewport 
	GlobalContext->UpdateViewport(InputAdapter);
}
