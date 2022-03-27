#include "FogOfWar/Upscaler/GK_Upscaler_Strategy.h"

#include "Engine/CanvasRenderTarget2D.h"

#include "Blueprint/GKUtilityLibrary.h"
#include "FogOfWar/GKFogOfWarVolume.h"


UGKTransformerStrategy::UGKTransformerStrategy()
{ 
    TextureSize = FIntVector::ZeroValue; 
    bInitialized = false;
    Multiplier   = 2;
}

void UGKTransformerStrategy::Initialize()
{ 
    FogOfWarVolume = Cast<AGKFogOfWarVolume>(GetOwner());
    bFixedSize     = FogOfWarVolume->bFixedSize;
    Multiplier     = FogOfWarVolume->Multiplier;
    FixedSize      = FogOfWarVolume->FixedSize;
    bUseUpscaledVision = FogOfWarVolume->bUseUpscaledVision;
}


void UGKTransformerStrategyTexture2D::Initialize() { 
    Super::Initialize();
    // TransformedTarget.Reset(); 
}

void UGKTransformerStrategyCanvas::Initialize() { 
    Super::Initialize();
    // TransformedTarget.Reset();  
}

UTexture2D *UGKTransformerStrategyTexture2D::GetFactionTransformTarget(FName name, bool bCreateRenderTarget)
{
    UTexture2D **Result  = TransformedTarget.Find(name);
    UTexture2D * Texture = nullptr;

    if (Result != nullptr)
    {
        Texture = Result[0];
    }
    else if (bCreateRenderTarget && !IsBeingDestroyed())
    {
        UE_LOG(LogGamekit, Log, TEXT("Creating a Transform Texture for faction %s"), *name.ToString());
        Texture = CreateTransformTarget();

        TransformedTarget.Add(name, Texture);
    }

    return Texture;
}

UTexture2D *UGKTransformerStrategyTexture2D::CreateTransformTarget()
{
    // FIXME: why is the strategy not initialized alread
    Initialize();

    auto Texture = UTexture2D::CreateTransient(
        UpscaledTextureSize().X, 
        UpscaledTextureSize().Y, 
        EPixelFormat::PF_G8
    );

    Texture->CompressionSettings = TextureCompressionSettings::TC_Default;
    Texture->SRGB                = false;
    Texture->Filter              = TextureFilter::TF_Default;
    Texture->AddressX            = TextureAddress::TA_Clamp;
    Texture->AddressY            = TextureAddress::TA_Clamp;
    Texture->MipGenSettings      = TextureMipGenSettings::TMGS_NoMipmaps;
    Texture->UpdateResource();

    return Texture;
}

class UTexture *UGKTransformerStrategyTexture2D::GetFactionTexture(FName name, bool CreateRenderTarget)
{
    return GetFactionTransformTarget(name, CreateRenderTarget);
}

UCanvasRenderTarget2D *UGKTransformerStrategyCanvas::GetFactionTransformTarget(FName name, bool bCreateRenderTarget)
{
    UCanvasRenderTarget2D **Result  = TransformedTarget.Find(name);
    UCanvasRenderTarget2D * Texture = nullptr;

    if (Result != nullptr)
    {
        Texture = Result[0];
    }
    else if (bCreateRenderTarget && !IsBeingDestroyed())
    {
        UE_LOG(LogGamekit, Log, TEXT("Creating a Transform Texture for faction %s"), *name.ToString());
        Texture = CreateTransformTarget();

        TransformedTarget.Add(name, Texture);
    }

    return Texture;
}

UCanvasRenderTarget2D *UGKTransformerStrategyCanvas::CreateTransformTarget()
{
    // FIXME: why is the strategy not initialized alread
    Initialize();

    auto Texture = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(
        GetWorld(),
        UCanvasRenderTarget2D::StaticClass(),
        UpscaledTextureSize().X,
        UpscaledTextureSize().Y
    );

    return Texture;
}

class UTexture *UGKTransformerStrategyCanvas::GetFactionTexture(FName name, bool CreateRenderTarget)
{
    return GetFactionTransformTarget(name, CreateRenderTarget);
}


