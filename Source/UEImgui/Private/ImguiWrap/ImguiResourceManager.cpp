#include "ImguiWrap/ImguiResourceManager.h"


#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "imgui.h"
#include "Logging.h"
#include "Config/ImguiConfig.h"
#include "ImguiWrap/ImguiContext.h"
#include "Misc/FileHelper.h"
#include "Slate/SlateTextureAtlasInterface.h"

FImguiResource::FImguiResource(const FName& InName, UTexture* SourceObject)
	: Name(InName)
	, Source(SourceObject)
{
	check(SourceObject != nullptr);
}

UImguiResourceManager::UImguiResourceManager()
	: CurrentResIdx(1)
	, DefaultFont(nullptr)
{
	_InitDefaultFont();
}

UImguiResourceManager& UImguiResourceManager::Get()
{
	static UImguiResourceManager* Ins = nullptr;
	if (!Ins)
	{
		Ins = NewObject<UImguiResourceManager>();
		Ins->AddToRoot();
	}

	return *Ins;
}

ImTextureID UImguiResourceManager::AddResource(FName InResName, UTexture* SourceObj)
{
	// find resource 
	auto FoundRes = NamedResourceMap.Find(InResName);
	if (FoundRes)
	{
		UE_LOG(LogUEImgui, Warning, TEXT("UImguiResourceManager::AddResource : resource %s has exist!!!"), *InResName.ToString());
		return reinterpret_cast<ImTextureID>((SIZE_T)*FoundRes);
	}

	// add resource
	int32 GotId = CurrentResIdx++;
	AllResource.Add(GotId, FImguiResource(InResName, (UTexture*)SourceObj));
	NamedResourceMap.Add(InResName, GotId);

	return reinterpret_cast<ImTextureID>((SIZE_T)GotId);
}

ImTextureID UImguiResourceManager::LoadResource(FString InPath, FName InName)
{
	FName ResourceName = InName == NAME_None ? FName(InPath) : InName;

	// find resource 
	auto FoundRes = NamedResourceMap.Find(ResourceName);
	if (FoundRes)
	{
		UE_LOG(LogUEImgui, Warning, TEXT("UImguiResourceManager::AddResource : resource %s has exist!!!"), *ResourceName.ToString());
		return reinterpret_cast<ImTextureID>((SIZE_T)*FoundRes);
	}

	// load texture
	// read file 
	TArray64<uint8>* Data = new TArray64<uint8>();
	FFileHelper::LoadFileToArray(*Data, *InPath);
	if (Data->Num() == 0) return NULL;

	// get image format 
	static const FName MODULE_IMAGE_WRAPPER("ImageWrapper");
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(MODULE_IMAGE_WRAPPER);
	EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(Data->GetData(), Data->Num());
	if (ImageFormat == EImageFormat::Invalid) return NULL;

	// decode image 
	FIntPoint Size;
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
	if (!ImageWrapper->SetCompressed(Data->GetData(), Data->Num())) return NULL;
	Size.X = ImageWrapper->GetWidth();
	Size.Y = ImageWrapper->GetHeight();
	ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, *Data);

	// create texture 
	UTexture2D* LoadedTexture = UTexture2D::CreateTransient(Size.X, Size.Y, EPixelFormat::PF_R8G8B8A8);
	LoadedTexture->UpdateResource();

	// set up region 
	FUpdateTextureRegion2D* TextureRegion = new FUpdateTextureRegion2D(
        0,
        0,
        0,
        0,
        Size.X,
        Size.Y);

	// cleanup data 
	auto DataCleanup = [FileData=Data](uint8* Data, const FUpdateTextureRegion2D* UpdateRegion)
	{
		delete FileData;
		delete UpdateRegion;
	};
	LoadedTexture->UpdateTextureRegions(0, 1u, TextureRegion, 4 * Size.X, 4, Data->GetData(), DataCleanup);

	// add resource
	int32 GotId = CurrentResIdx++;
	AllResource.Add(GotId, FImguiResource(ResourceName, LoadedTexture));
	NamedResourceMap.Add(ResourceName, GotId);

	return reinterpret_cast<ImTextureID>((SIZE_T)GotId);
}

bool UImguiResourceManager::IsResourceExist(FName InResName)
{
	return NamedResourceMap.Find(InResName) != nullptr;
}

bool UImguiResourceManager::IsResourceExist(ImTextureID InID)
{
	return AllResource.Find((int32)reinterpret_cast<SIZE_T>(InID)) != nullptr;
}

FImguiResource* UImguiResourceManager::FindResource(FName InResName)
{
	auto FoundID = NamedResourceMap.Find(InResName);
	if (!FoundID) return nullptr;
	return AllResource.Find(*FoundID);
}

FImguiResource* UImguiResourceManager::FindResource(ImTextureID InResId)
{
	return AllResource.Find((int32)reinterpret_cast<SIZE_T>(InResId));
}

void UImguiResourceManager::ReleaseResource(FName InResName)
{
	auto FoundRes = NamedResourceMap.Find(InResName);
	if (FoundRes)
	{
		AllResource.Remove(*FoundRes);
		NamedResourceMap.Remove(InResName);
	}
}

void UImguiResourceManager::ReleaseResource(ImTextureID InID)
{
	int32 UEId = (int32)reinterpret_cast<SIZE_T>(InID);
	auto FoundRes = AllResource.Find(UEId);
	if (FoundRes)
	{
		NamedResourceMap.Remove(FoundRes->Name);
		AllResource.Remove(UEId);
	}
}

void UImguiResourceManager::BeginDestroy()
{
	Super::BeginDestroy();
	_ShutDownDefaultFont();
}

void UImguiResourceManager::_InitDefaultFont()
{
	DefaultFont = IM_NEW(ImFontAtlas);	

	// get unreal default font 
#if WITH_EDITOR
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
#else
	if (UImguiConfig::Get()->FontPath.IsEmpty())
	{
		DefaultFont->AddFontDefault();
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
#endif
	
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

void UImguiResourceManager::_ShutDownDefaultFont()
{
	IM_DELETE(DefaultFont);
}
