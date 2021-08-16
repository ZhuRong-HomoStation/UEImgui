#include "Services/ImguiGlobalContextService.h"
#include "ImguiPerInstanceCtx.h"
#include "imgui_internal.h"
#include "Logging.h"
#include "Config/ImguiConfig.h"
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
    	FCoreDelegates::OnPostEngineInit.AddLambda([this]
    	{
			if(FSlateApplication::IsInitialized())
    		{
    			FSlateApplication::Get().OnPreTick().AddRaw(this, &FEditorGlobalContextGuard::Tick);
			}
    	});

    	FCoreDelegates::OnEnginePreExit.AddLambda([this]
    	{
    		if (UImguiConfig::Get()->bSaveLayout)
    		{
    			SaveLayout();
    		}
    	});
    }

	void SaveLayout()
    {
	    // save layout 
        FString LayoutSettingDir = FPaths::ProjectConfigDir() / TEXT("ImguiLayout_Engine.ini");
        auto OldContext = ImGui::GetCurrentContext();
        Context->ApplyContext();
        ImGui::SaveIniSettingsToDisk(TCHAR_TO_UTF8(*LayoutSettingDir));
        ImGui::SetCurrentContext(OldContext);
    }

	void Tick(float DeltaTime)
	{
		if (!GEngine->IsInitialized()) return;
    	
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
			
			// create proxy
			TSharedPtr<SImguiRenderProxy> Proxy = SNew(SImguiRenderProxy)
            .InContext(Context)
            .InAdapter(InputAdapter)
            .HSizingRule(EImguiSizingRule::UESize)
            .VSizingRule(EImguiSizingRule::UESize)
            .BlockInput(true)
			.Visibility(EVisibility::HitTestInvisible);
			
			// add to viewport
			if (AllViewportClients.Num() != 0)
			{
				for (FLevelEditorViewportClient* Client : AllViewportClients)
				{
					Client->GetEditorViewportWidget()->ViewportOverlay->AddSlot()
	                [
                		Proxy->AsShared()
	                ];
				}
			}

			// init context
			Context->Init(Proxy, UImguiResourceManager::Get().GetDefaultFont());

			// enable docking and viewport 
			Context->EnableDocking(true);
			Context->EnableViewport(true);
			Context->EnableDPIScale(true);
			Context->EnableNoAutoMergeViewport(true);
			
			// set viewport manually 
			StaticCastSharedPtr<IImguiViewport>(Proxy)->SetupViewport(Context->GetContext()->Viewports[0]);

			// load layout
			if (UImguiConfig::Get()->bSaveLayout)
			{
				FString LayoutSettingDir = FPaths::ProjectConfigDir() / TEXT("ImguiLayout_Engine.ini");
				auto OldContext = ImGui::GetCurrentContext();
				Context->ApplyContext();
				ImGui::LoadIniSettingsFromDisk(TCHAR_TO_UTF8(*LayoutSettingDir));
				ImGui::SetCurrentContext(OldContext);
			}
			
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

    	// save layout
    	if (UImguiConfig::Get()->bSaveLayout)
    	{
    		static float AccumulateTime = 0.f;
    		AccumulateTime += DeltaTime;
    		if (AccumulateTime >= 30.f)
    		{
    			SaveLayout();
    			AccumulateTime -= 30.f;
    		}
    	}
	}
	
	UImguiContext* Context = nullptr;
	UImguiInputAdapterDeferred* InputAdapter = nullptr;
};

static FEditorGlobalContextGuard Ins;
static FEditorGlobalContextGuard* EngineGlobalCtxGuard()
{
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
	if (!TimeToDraw())
	{
		UE_LOG(LogUEImgui, Error, TEXT("ImGui context invalid, AddGlobalWindow failed!!!"));
		return INDEX_NONE;
	}
		
	UImguiContext* Ctx = GetGlobalContext(WorldContextObject);
	return Ctx->AddGlobalWindow(InGlobalContext);
}

void UEImGui::RemoveGlobalWindow(int32 InIndex, UObject* WorldContextObject)
{
	if (!TimeToDraw() || InIndex == INDEX_NONE)
	{
		UE_LOG(LogUEImgui, Error, TEXT("ImGui context invalid, RemoveGlobalWindow failed!!!"));
		return;
	}		
	UImguiContext* Ctx = GetGlobalContext(WorldContextObject);
	Ctx->RemoveGlobalWindow(InIndex);
}

void UEImGui::AddRenderProxy(TWeakPtr<IImguiViewport> InRenderProxy, UObject* WorldContextObject)
{
	if (!TimeToDraw())
	{
		UE_LOG(LogUEImgui, Error, TEXT("ImGui context invalid, AddRenderProxy failed!!!"));
		return;
	}
	UImguiContext* Ctx = GetGlobalContext(WorldContextObject);
	Ctx->AddRenderProxy(InRenderProxy);
}

void UEImGui::RemoveRenderProxy(TWeakPtr<IImguiViewport> InRenderProxy, UObject* WorldContextObject)
{
	if (!TimeToDraw())
	{
		UE_LOG(LogUEImgui, Error, TEXT("ImGui context invalid, RemoveRenderProxy failed!!!"));
		return;
	}
	UImguiContext* Ctx = GetGlobalContext(WorldContextObject);
	Ctx->RemoveRenderProxy(InRenderProxy);
}

