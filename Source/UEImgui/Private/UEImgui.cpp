#include "UEImgui.h"
#include "imgui_internal.h"
#include "Logging.h"
#include "Config/ImguiConfig.h"
#include "ImguiInput/ImguiInputAdapter.h"

ImTextureID UUEImgui::AddResource(FName InResName, UTexture* SourceObj)
{
	// find resource 
	auto FoundRes = NamedResourceMap.Find(InResName);
	if (FoundRes)
	{
		UE_LOG(LogUEImgui, Warning, TEXT("UUEImgui::AddResource : resource %s has exist!!!"), *InResName.ToString());
		return reinterpret_cast<ImTextureID>((SIZE_T)*FoundRes);
	}

	// add resource
	int32 GotId = CurrentResIdx++;
	AllResource.Add(GotId, FImguiResource(InResName, (UTexture*)SourceObj));
	NamedResourceMap.Add(InResName, GotId);

	return reinterpret_cast<ImTextureID>((SIZE_T)GotId);
}

bool UUEImgui::IsResourceExist(FName InResName)
{
	return NamedResourceMap.Find(InResName) != nullptr;
}

bool UUEImgui::IsResourceExist(ImTextureID InID)
{
	return AllResource.Find((int32)reinterpret_cast<SIZE_T>(InID)) != nullptr;
}

FImguiResource* UUEImgui::FindResource(FName InResName)
{
	auto FoundID = NamedResourceMap.Find(InResName);
	if (!FoundID) return nullptr;
	return AllResource.Find(*FoundID);
}

FImguiResource* UUEImgui::FindResource(ImTextureID InResId)
{
	return AllResource.Find((int32)reinterpret_cast<SIZE_T>(InResId));
}

void UUEImgui::ReleaseResource(FName InResName)
{
	auto FoundRes = NamedResourceMap.Find(InResName);
	if (FoundRes)
	{
		AllResource.Remove(*FoundRes);
		NamedResourceMap.Remove(InResName);
	}
}

void UUEImgui::ReleaseResource(ImTextureID InID)
{
	int32 UEId = (int32)reinterpret_cast<SIZE_T>(InID);
	auto FoundRes = AllResource.Find(UEId);
	if (FoundRes)
	{
		NamedResourceMap.Remove(FoundRes->Name);
		AllResource.Remove(UEId);
	}
}

void UUEImgui::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// init default font
	_InitDefaultFont();
	
	// init global context
	_InitGlobalContext();
}

void UUEImgui::Deinitialize()
{
	Super::Deinitialize();

	// release default font
	_ShutDownDefaultFont();

	// release default context 
	_ShutDownGlobalContext();
}

void UUEImgui::_InitGlobalContext()
{
	// please init font first 
	check(DefaultFont);

	// create context 
	GlobalContext = ImGui::CreateContext(DefaultFont);

	// enable keyboard control
	GlobalContext->IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// set backend flags
	GlobalContext->IO.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	GlobalContext->IO.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
	GlobalContext->IO.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
	GlobalContext->IO.BackendPlatformName = "Unreal Backend";
	GlobalContext->IO.BackendPlatformName = "Unreal Widget";

	// set up style
	ImGui::StyleColorConfig(&GlobalContext->Style);

	// disable .ini by default
	GlobalContext->IO.IniFilename = nullptr;

	// set up key map 
	UImguiInputAdapter::CopyUnrealKeyMap(&GlobalContext->IO);

	// capture display device info
	FDisplayMetrics DisplayMetrics;
	FDisplayMetrics::RebuildDisplayMetrics(DisplayMetrics);
	GlobalContext->IO.DisplaySize = {
		(float)DisplayMetrics.VirtualDisplayRect.Right - DisplayMetrics.VirtualDisplayRect.Left,
		(float)DisplayMetrics.VirtualDisplayRect.Bottom - DisplayMetrics.VirtualDisplayRect.Top };
}

void UUEImgui::_ShutDownGlobalContext()
{
	ImGui::DestroyContext(GlobalContext);
	GlobalContext = nullptr;
}

void UUEImgui::_InitDefaultFont()
{
	DefaultFont = IM_NEW(ImFontAtlas);	

	if (UImguiConfig::Get()->FontPath.IsEmpty())
	{
		// get unreal default font 
		const FCompositeFont* CompositeFont = FCoreStyle::Get().
		GetWidgetStyle<FTextBlockStyle>("NormalText").Font.GetCompositeFont();
		FString FontPath = CompositeFont->DefaultTypeface.Fonts[0].Font.GetFontFilename();
		FString FallbackPath = CompositeFont->FallbackTypeface.Typeface.Fonts[0].Font.GetFontFilename();
		
		// add font
		DefaultFont->AddFontFromFileTTF(
 			TCHAR_TO_UTF8(*FontPath), 15.0f, nullptr, DefaultFont->GetGlyphRangesDefault());
		ImFontConfig FontConfig = {};
		FontConfig.MergeMode = true;
		FontConfig.SizePixels = 15.0f;
		DefaultFont->AddFontFromFileTTF(
			TCHAR_TO_UTF8(*FallbackPath), 15.0f, &FontConfig, DefaultFont->GetGlyphRangesChineseFull());
	}
	else
	{
		DefaultFont->AddFontFromFileTTF(
             TCHAR_TO_UTF8(*UImguiConfig::Get()->FontPath), 15.0f, nullptr, DefaultFont->GetGlyphRangesDefault());
		ImFontConfig FontConfig = {};
		FontConfig.MergeMode = true;
		FontConfig.SizePixels = 15.0f;
		DefaultFont->AddFontFromFileTTF(
            TCHAR_TO_UTF8(*UImguiConfig::Get()->FontPath), 15.0f, &FontConfig, DefaultFont->GetGlyphRangesChineseFull());
	}
	
	// get texture info 
	unsigned char* Pixels;
	int Width, Height, Bpp;
	DefaultFont->GetTexDataAsRGBA32(&Pixels, &Width, &Height, &Bpp);

	// copy to UTexture2D 
	UTexture2D* Texture = UTexture2D::CreateTransient(Width, Height, EPixelFormat::PF_B8G8R8A8,
		TEXT("Imgui_DefaultFont"));
	Texture->UpdateResource();
	FUpdateTextureRegion2D* TextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, Width, Height);
	auto DataCleanup = [](uint8* Data, const FUpdateTextureRegion2D* UpdateRegion)
	{
		delete UpdateRegion;
	};
	Texture->UpdateTextureRegions(0, 1u, TextureRegion, Bpp * Width, Bpp, Pixels, DataCleanup);

	// add to resource map
	AllResource.Add((int32)EGlobalImguiTextureId::DefaultFont, FImguiResource("DefaultFont", Texture));
	NamedResourceMap.Add("DefaultFont", (int32)EGlobalImguiTextureId::DefaultFont);

	// setup font
	DefaultFont->TexID = reinterpret_cast<ImTextureID>(EGlobalImguiTextureId::DefaultFont);
}

void UUEImgui::_ShutDownDefaultFont()
{
	IM_DELETE(DefaultFont);
}
