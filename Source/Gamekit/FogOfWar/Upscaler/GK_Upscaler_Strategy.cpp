#include "Gamekit/FogOfWar/Upscaler/GK_Upscaler_Strategy.h"

#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"


UGKUpscalerStrategy::UGKUpscalerStrategy() { 
    TextureSize = FIntVector::ZeroValue; 
    bInitialized = false;
    Multiplier   = 2;
}

void UGKUpscalerStrategy::Initialize()
{ 
    FogOfWarVolume = Cast<AGKFogOfWarVolume>(GetOwner());
}

UpscaledTextureType *UGKUpscalerStrategy::GetFactionUpscaleTarget(FName name, bool bCreateRenderTarget)
{
    UpscaledTextureType **Result  = UpscaledFogFactions.Find(name);
    UpscaledTextureType * Texture = nullptr;

    if (Result != nullptr)
    {
        Texture = Result[0];
    }
    else if (bCreateRenderTarget)
    {
        UE_LOG(LogGamekit, Log, TEXT("Creating a Upscale Texture for faction %s"), *name.ToString());
        Texture = CreateUpscaleTarget();

        UpscaledFogFactions.Add(name, Texture);
    }

    return Texture;
}

UpscaledTextureType *UGKUpscalerStrategy::CreateUpscaleTarget()
{
    // FIXME: why is the strategy not initialized alread
    Initialize();

    auto Texture = UTexture2D::CreateTransient(
        TextureSize.X * Multiplier, 
        TextureSize.Y * Multiplier, 
        EPixelFormat::PF_G8
    );

    Texture->CompressionSettings = TextureCompressionSettings::TC_Default;
    Texture->SRGB                = false;
    Texture->Filter              = TextureFilter::TF_Nearest;
    Texture->AddressX            = TextureAddress::TA_Clamp;
    Texture->AddressY            = TextureAddress::TA_Clamp;
    Texture->MipGenSettings      = TextureMipGenSettings::TMGS_NoMipmaps;

    Texture->UpdateResource();
    return Texture;
}

class UTexture *UGKUpscalerStrategy::GetFactionTexture(FName name, bool CreateRenderTarget)
{
    return GetFactionUpscaleTarget(name, CreateRenderTarget);
}
