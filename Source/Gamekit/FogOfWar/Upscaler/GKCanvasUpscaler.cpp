// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/FogOfWar/Upscaler/GKCanvasUpscaler.h"

// Gamekit
#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"
#include "Gamekit/FogOfWar/GKFogOfWarActorTeam.h"

// Unreal Engine
#include "Engine/Canvas.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UGKCanvasUpscaler::Transform(class AGKFogOfWarActorTeam *FactionFog)
{
    if (UpscalingMaterial == nullptr)
    {
        UE_LOG(LogGamekit, Warning, TEXT("No upscaling material set"));
        return;
    }

    UCanvas *                  Canvas;
    FVector2D                  Size;
    FDrawToRenderTargetContext Context;

    auto RenderCanvas          = GetFactionTransformTarget(FactionFog->Name, true);
    FactionFog->UpScaledVision = RenderCanvas;

    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), RenderCanvas, Canvas, Size, Context);

    auto Material = GetFactionMaterialInstance(FactionFog->Name);
    Material->SetTextureParameterValue("Texture", FactionFog->Vision);
    Material->SetTextureParameterValue("Previous", FactionFog->PreviousFrameVision);
    Material->SetScalarParameterValue("IsDiscrete", FactionFog->bDiscrete);

    Canvas->K2_DrawMaterial(Material, FVector2D::ZeroVector, Size, FVector2D::ZeroVector);

    UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), Context);
}

class UMaterialInstanceDynamic *UGKCanvasUpscaler::GetFactionMaterialInstance(FName Name)
{
    class UMaterialInstanceDynamic **Result   = UpscaleMaterialInstance.Find(Name);
    class UMaterialInstanceDynamic * Instance = nullptr;

    if (Result != nullptr)
    {
        Instance = Result[0];
    }
    else
    {
        Instance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(
                GetWorld(), UpscalingMaterial, NAME_None, EMIDCreationFlags::None);

        UpscaleMaterialInstance.Add(Name, Instance);
    }
    return Instance;
}

void UGKCanvasUpscaler::Initialize()
{
    Super::Initialize();
    UpscalingMaterial = FogOfWarVolume->UpscaleMaterial;
}
