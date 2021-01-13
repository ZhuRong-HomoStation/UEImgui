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
class FEditorGlobalContextGuard : public FTickableEditorObject
{
public:	
	virtual void Tick(float DeltaTime) override
	{		
		// create imgui context 
		if (!Context)
		{
			Context = NewObject<UImguiContext>();
			InputAdapter = NewObject<UImguiInputAdapterDeferred>();
			Context->AddToRoot();
			InputAdapter->AddToRoot();
			InputAdapter->SetContext(Context);
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
			TSharedPtr<SImguiWidgetRenderProxy> Proxy = SNew(SImguiWidgetRenderProxy)
            .InContext(Context)
            .InAdapter(InputAdapter)
            .HSizingRule(EImguiSizingRule::UESize)
            .VSizingRule(EImguiSizingRule::UESize)
            .BlockInput(false);
			
			// add to viewport
			for (FLevelEditorViewportClient* Client : AllViewportClients)
			{
				Client->GetEditorViewportWidget()->ViewportOverlay->AddSlot()
                [
                    Proxy->AsShared()
                ];
			}

			// init context
			Context->Init(Proxy, UImguiResourceManager::Get().GetDefaultFont());

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
		Context->NewFrame();

		// draw global
		Context->DrawGlobal();

		// render
		Context->Render();

		// update viewport 
		Context->UpdateViewport(InputAdapter);
	}
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(FEditorGlobalContextGuard, STATGROUP_Tickables); }
	
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

