#include "Services/ImguiGlobalContextService.h"
#include "imgui_internal.h"
#include "Framework/Application/IInputProcessor.h"
#include "ImguiWrap/ImguiContext.h"
#include "ImguiWrap/ImguiHelp.h"
#include "ImguiWrap/ImguiInputAdapterDeferred.h"
#include "Widgets/SImguiWindow.h"
#include "ImguiWrap/ImguiResourceManager.h"
#include "ImguiWrap/ImguiTextInputSystem.h"
#include "ImguiWrap/ImguiUEWrap.h"
#include "Widgets/SToolTip.h"
#include "Widgets/Images/SImage.h"
#include "Window/ImguiWindowWrapper.h"

#if WITH_EDITOR
#include "Interfaces/IMainFrameModule.h"
#endif 

UImguiGlobalContextService::UImguiGlobalContextService()
	: GlobalContext(nullptr)
{
}

bool UImguiGlobalContextService::TimeToDraw()
{
	return InputAdapter != nullptr && GlobalContext != nullptr;
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
	GlobalContext->ShutDown();
}

void UImguiGlobalContextService::_OnSlatePreTick(float DeltaTime)
{
	if (!GlobalContext)
	{
		// create render proxy
		TSharedPtr<SImguiWidgetRenderProxy> Proxy = SNew(SImguiWidgetRenderProxy);
		
		IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
		const TSharedPtr<SWindow> MainFrameWindow = MainFrame.GetParentWindow();

		auto OldContent = MainFrameWindow->GetContent();

		MainFrameWindow->SetContent(SNew(SOverlay)
			+ SOverlay::Slot()
			[
				Proxy->AsShared()
			]
			+ SOverlay::Slot()
			[
				ConstCastSharedRef<SWidget>(OldContent)
			]);
		
		// create global context 
		GlobalContext = NewObject<UImguiContext>();
		GlobalContext->Init(Proxy, UImguiResourceManager::Get().GetDefaultFont());
		
		// create adapter
		if (!InputAdapter) InputAdapter = NewObject<UImguiInputAdapterDeferred>();
		InputAdapter->SetContext(GlobalContext);

		// add to global input hook
		GlobalInputHook->AddAdapter(InputAdapter);
		return;
	}

	// set up context info
	GlobalContext->GetIO()->DeltaTime = DeltaTime;
	
	// apply context 
	GlobalContext->ApplyContext();

	// end listen
	FImguiTextInputSystem::Get()->EndListen();

	// apply input 
	InputAdapter->ApplyInput();
	InputAdapter->SaveTempData();

	// begin frame
	GlobalContext->NewFrame();

	// draw global
	GlobalContext->DrawGlobal();

	// render
	GlobalContext->Render();

	// update viewport
	FImguiWindowWrapper::CurInputAdapter = InputAdapter;
	GlobalContext->UpdateViewport(InputAdapter);

	// begin listen
	FImguiTextInputSystem::Get()->BeginListen();
}

