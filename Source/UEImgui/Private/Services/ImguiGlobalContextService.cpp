#include "Services/ImguiGlobalContextService.h"
#include "ImguiPerInstanceCtx.h"
#include "imgui_internal.h"
#include "ImguiWrap/ImguiContext.h"
#include "ImguiWrap/ImguiGlobalInputHook.h"
#include "ImguiWrap/ImguiInputAdapter.h"
#include "ImguiWrap/ImguiInputAdapterDeferred.h"
#include "ImguiWrap/ImguiResourceManager.h"

#if WITH_EDITOR
#define protected public
#define private public 
#include "LevelEditorViewport.h"
#include "LevelEditor.h"
#include "ILevelEditor.h"
#include "ILevelViewport.h"
#include "SEditorViewport.h"
class FEditorGlobalContextGuard
{
public:
    FEditorGlobalContextGuard()
    {
    	FSlateApplication::Get().OnPreTick().AddRaw(this, &FEditorGlobalContextGuard::Tick);
    }
	void Tick(float DeltaTime)
	{		
		// create imgui context 
		if (!Context)
		{
			// create context 
			Context = NewObject<UImguiContext>();
			Context->AddToRoot();

			// create input adapter 
			InputAdapter = NewObject<UImguiInputAdapterDeferred>();
			InputAdapter->AddToRoot();
			InputAdapter->SetContext(Context);
			
			// setup default adapter
			Context->SetDefaultInputAdapter(InputAdapter);
			return;
		}

		// add global hook 
		static bool bHookAdded = false;
		if (!bHookAdded)
		{
			auto GlobalHook = FImguiGlobalInputHook::Get();
			if (!GlobalHook.IsValid()) return;
			GlobalHook->AddAdapter(InputAdapter);
			bHookAdded = true;
		}
		
		// try to init
		if (!Context->IsInit())
		{
			// find viewport widget 
			auto& AllViewportClients = GEditor->GetLevelViewportClients();
			if (AllViewportClients.Num() == 0) return;
			
			// create proxy
			TSharedPtr<SImguiRenderProxy> Proxy = SNew(SImguiRenderProxy)
            .InContext(Context)
            .InAdapter(InputAdapter)
            .HSizingRule(EImguiSizingRule::UESize)
            .VSizingRule(EImguiSizingRule::UESize)
            .BlockInput(true);
			
			// add to viewport
			for (FLevelEditorViewportClient* Client : AllViewportClients)
			{
				Client->GetEditorViewportWidget()->ViewportOverlay->AddSlot(0)
                [
                    Proxy->AsShared()
                ];
			}

			// init context
			Context->Init(Proxy, UImguiResourceManager::Get().GetDefaultFont());

			// enable docking and viewport 
			Context->EnableDocking(true);
			Context->EnableViewport(true);
			Context->EnableNoAutoMergeViewport(true);
			
			// set viewport manually 
			StaticCastSharedPtr<IImguiViewport>(Proxy)->SetupViewport(Context->GetContext()->Viewports[0]);

			return;
		}

		// update main viewport size
		Context->UpdateSize();

		// set up context info
		Context->GetIO()->DeltaTime = DeltaTime;
	
		// apply context 
		Context->ApplyContext();

		// apply input 
		InputAdapter->ApplyInput();
		InputAdapter->SaveTempData();
		
		// begin frame
		Context->NewFrame(DeltaTime);

		// draw global
		Context->DrawGlobal();

		// render
		Context->Render();

		// update viewport 
		Context->UpdateViewport(InputAdapter);
	}
	
	UImguiContext* Context = nullptr;
	UImguiInputAdapterDeferred* InputAdapter = nullptr;
};
static FEditorGlobalContextGuard* EngineGlobalCtxGuard()
{
	static FEditorGlobalContextGuard Ins;
	return &Ins;
}
#undef protected
#undef private 
#endif

bool UEImGui::TimeToDraw(UObject* WorldContextObject)
{
	UImguiContext* Ctx = GetGlobalContext(WorldContextObject);
	return Ctx ? Ctx->IsInit() : false;
}

UImguiContext* UEImGui::GetGlobalContext(UObject* WorldContextObject)
{
	UWorld* FoundWorld = WorldContextObject ? WorldContextObject->GetWorld() : GWorld;
	if (!FoundWorld) return nullptr;
	UGameInstance* GameInst = FoundWorld->GetGameInstance();
	if (GameInst)
	{
		auto PerInsCtx = GameInst->GetSubsystem<UImguiPerInstanceCtx>();
		if (!PerInsCtx) return nullptr;
		return PerInsCtx->GetGlobalContext();
	}
	else
	{
#if WITH_EDITOR
		return EngineGlobalCtxGuard()->Context;
#else 
		return nullptr;
#endif 	
	}
}

UImguiInputAdapter* UEImGui::GetGlobalInputAdapter(UObject* WorldContextObject)
{
	UWorld* FoundWorld = WorldContextObject ? WorldContextObject->GetWorld() : GWorld;
	if (!FoundWorld) return nullptr;
	UGameInstance* GameInst = FoundWorld->GetGameInstance();
	if (GameInst)
	{
		auto PerInsCtx = GameInst->GetSubsystem<UImguiPerInstanceCtx>();
		if (!PerInsCtx) return nullptr;
		return PerInsCtx->GetGlobalInputAdapter();
	}
	else
	{
#if WITH_EDITOR
		return EngineGlobalCtxGuard()->InputAdapter;
#else 
		return nullptr;
#endif 	
	}
}

int32 UEImGui::AddGlobalWindow(const FDrawGlobalImgui& InGlobalContext, UObject* WorldContextObject)
{
	check(TimeToDraw());
	UImguiContext* Ctx = GetGlobalContext(WorldContextObject);
	return Ctx->AddGlobalWindow(InGlobalContext);
}

void UEImGui::RemoveGlobalWindow(int32 InIndex, UObject* WorldContextObject)
{
	check(TimeToDraw());
	UImguiContext* Ctx = GetGlobalContext(WorldContextObject);
	Ctx->RemoveGlobalWindow(InIndex);
}

void UEImGui::AddRenderProxy(TWeakPtr<IImguiViewport> InRenderProxy, UObject* WorldContextObject)
{
	check(TimeToDraw());
	UImguiContext* Ctx = GetGlobalContext(WorldContextObject);
	Ctx->AddRenderProxy(InRenderProxy);
}

void UEImGui::RemoveRenderProxy(TWeakPtr<IImguiViewport> InRenderProxy, UObject* WorldContextObject)
{
	check(TimeToDraw());
	UImguiContext* Ctx = GetGlobalContext(WorldContextObject);
	Ctx->RemoveRenderProxy(InRenderProxy);
}

