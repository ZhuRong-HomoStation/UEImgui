#include "ImguiWrap/ImguiContext.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "ImguiWrap/ImguiInputAdapter.h"
#include "ImguiWrap/ImguiUEWrap.h"

void UImguiContext::BeginDestroy()
{
	Super::BeginDestroy();
	
	ShutDown();
}

UImguiContext::UImguiContext(const FObjectInitializer& InInitializer)
{
}

void UImguiContext::Init(ImFontAtlas* InDefaultFontAtlas)
{
	// create context 
	Context = ImGui::CreateContext(InDefaultFontAtlas);

	// set up context
	_SetupImguiContext();

	// set up key map 
	UImguiInputAdapter::CopyUnrealKeyMap(GetIO());
}

void UImguiContext::Reset()
{
}

void UImguiContext::ShutDown()
{
	// destroy context 
	if (Context) ImGui::DestroyContext(Context);

	// clean reference
	Context = nullptr;
}

void UImguiContext::ApplyContext()
{
	ImGui::SetCurrentContext(Context);
}

ImGuiIO* UImguiContext::GetIO() const
{
	return &Context->IO;
}

ImGuiStyle* UImguiContext::GetStyle() const
{
	return &Context->Style;
}

void UImguiContext::_SetupImguiContext()
{
	// enable keyboard control
	GetIO()->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// set backend flags
	GetIO()->BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	GetIO()->BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
	GetIO()->BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
	GetIO()->BackendPlatformName = "Unreal Backend";
	GetIO()->BackendPlatformName = "Unreal Widget";

	// set up style
	ImGui::StyleColorConfig(&GetContext()->Style);

	// disable .ini by default
	GetIO()->IniFilename = nullptr;
}

ImFontAtlas& GetDefaultFontAtlas()
{
	static ImFontAtlas FontAtlas;
	if (!FontAtlas.IsBuilt())
	{
		ImFontConfig FontConfig = {};
		FontConfig.SizePixels = FMath::RoundFromZero(13.f);
		FontAtlas.AddFontDefault(&FontConfig);

		unsigned char* Pixels;
		int Width, Height, Bpp;
		FontAtlas.GetTexDataAsRGBA32(&Pixels, &Width, &Height, &Bpp);
	}
	return FontAtlas;
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
