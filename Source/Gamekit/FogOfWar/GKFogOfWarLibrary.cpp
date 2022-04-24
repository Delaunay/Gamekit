// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/FogOfWar/GKFogOfWarLibrary.h"

// Gamekit
#include "Gamekit/FogOfWar/GKFogOfWar.h"
#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"

// Unreal Engine
#include "Camera/CameraComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"

class UCollisionProfileHack
{
    public:
    TArray<ECollisionChannel> ObjectTypeMapping;
};

void UGKFogOfWarLibrary::ConvertToObjectType(ECollisionChannel                      CollisionChannel,
                                             TArray<TEnumAsByte<EObjectTypeQuery>> &ObjectTypes)
{
    if (CollisionChannel < ECC_MAX)
    {
        int32 ObjectTypeIndex = 0;
        // ofc it is private
        UCollisionProfileHack *CollisionProfile = reinterpret_cast<UCollisionProfileHack *>(UCollisionProfile::Get());

        for (const auto &MappedCollisionChannel: CollisionProfile->ObjectTypeMapping)
        {
            if (MappedCollisionChannel == CollisionChannel)
            {
                ObjectTypes.Add((EObjectTypeQuery)ObjectTypeIndex);
            }

            ObjectTypeIndex++;
        }
    }
}

void UGKFogOfWarLibrary::SetCameraPostprocessMaterial(AGKFogOfWarVolume *Volume,
                                                      FName              Faction,
                                                      UCameraComponent  *CameraComponent)
{
    auto Material = Volume->GetFogOfWarPostprocessMaterial(Faction);

    if (Material == nullptr)
    {
        GKFOG_WARNING(TEXT("Postprocess Material was null, Are you using decal rendering?"));
        return;
    }

    for (auto &Blendable: CameraComponent->PostProcessSettings.WeightedBlendables.Array)
    {
        if (Blendable.Object == Material)
        {
            return;
        }
    }
    CameraComponent->PostProcessSettings.WeightedBlendables.Array.Add(FWeightedBlendable(1, Material));
}

bool UGKFogOfWarLibrary::IsVisible(UObject *WorldCtx, AActor const *Target)
{
    UWorld *World = GEngine->GetWorldFromContextObject(WorldCtx, EGetWorldErrorMode::LogAndReturnNull);
    if (!World)
    {
        return false;
    }
    ULocalPlayer *Player = World->GetFirstLocalPlayerFromController();
    if (!Player)
    {
        return false;
    }

    auto Controller = Player->GetPlayerController(World);
    if (!Controller)
    {
        return false;
    }

    TArray<AActor *> OutActors;
    UGameplayStatics::GetAllActorsOfClass(World, AGKFogOfWarVolume::StaticClass(), OutActors);

    auto FogOfWarVolume = Cast<AGKFogOfWarVolume>(OutActors[0]);

    return FogOfWarVolume->IsVisible(FGenericTeamId::GetTeamIdentifier(Controller), Target);
}

FLinearColor UGKFogOfWarLibrary::SampleRenderTarget(UTextureRenderTarget2D *InRenderTarget, FVector2D UV)
{
    if (!InRenderTarget)
    {
        GKFOG_WARNING(TEXT("SampleRenderTarget::RenderTarget is null"));
        return FLinearColor(0, 0, 0, 0);
    }

    UV.X *= InRenderTarget->SizeX;
    UV.Y *= InRenderTarget->SizeY;

    return SamplePixelRenderTarget(InRenderTarget, UV);
}

FLinearColor UGKFogOfWarLibrary::SamplePixelRenderTarget(UTextureRenderTarget2D *InRenderTarget, FVector2D Pixel)
{
    if (!InRenderTarget)
    {
        GKFOG_WARNING(TEXT("SampleRenderTarget::RenderTarget is null"));
        return FLinearColor(0, 0, 0, 0);
    }

    if (!InRenderTarget->GetResource())
    {
        GKFOG_WARNING(TEXT("SampleRenderTarget::RenderTarget.Resources was released"));
        return FLinearColor(0, 0, 0, 0);
    }

    ETextureRenderTargetFormat format = (InRenderTarget->RenderTargetFormat);
    if ((format == (RTF_RGBA16f)) || (format == (RTF_RGBA32f)) || (format == (RTF_RGBA8)))
    {
        FTextureRenderTargetResource *RTResource = InRenderTarget->GameThread_GetRenderTargetResource();

        Pixel.X = FMath::Clamp(Pixel.X, 0.0f, float(InRenderTarget->SizeX) - 1);
        Pixel.Y = FMath::Clamp(Pixel.Y, 0.0f, float(InRenderTarget->SizeY) - 1);

        FIntRect              Rect = FIntRect(Pixel.X, Pixel.Y, Pixel.X + 1, Pixel.Y + 1);
        FReadSurfaceDataFlags ReadPixelFlags(RCM_MinMax);

        TArray<FColor>       OutLDR;
        TArray<FLinearColor> OutHDR;
        TArray<FLinearColor> OutVals;

        bool ishdr = ((format == (RTF_R16f)) || (format == (RTF_RG16f)) || (format == (RTF_RGBA16f)) ||
                      (format == (RTF_R32f)) || (format == (RTF_RG32f)) || (format == (RTF_RGBA32f)));

        if (!ishdr)
        {
            RTResource->ReadPixels(OutLDR, ReadPixelFlags, Rect);
            for (auto i: OutLDR)
            {
                OutVals.Add(FLinearColor(float(i.R), float(i.G), float(i.B), float(i.A)) / 255.0f);
            }
        }
        else
        {
            RTResource->ReadLinearColorPixels(OutHDR, ReadPixelFlags, Rect);
            return OutHDR[0];
        }

        return OutVals[0];
    }

    GKFOG_WARNING(TEXT("SampleRenderTarget Unsupported pixel format %d"), int(format));
    return FLinearColor(0, 0, 0, 0);
}