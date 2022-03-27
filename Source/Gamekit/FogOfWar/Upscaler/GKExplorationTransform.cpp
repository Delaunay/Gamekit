// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.


#include "FogOfWar/Upscaler/GKExplorationTransform.h"

#include "FogOfWar/GKFogOfWarVolume.h"

#include "Engine/Canvas.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"


void UGKExplorationTransform::Initialize()
{
    Super::Initialize();
}


void UGKExplorationTransform::Transform(FGKFactionFog *FactionFog)
{
    UCanvas *                  Canvas;
    FVector2D                  Size;
    FDrawToRenderTargetContext Context;

    UCanvasRenderTarget2D* RenderTarget       = GetFactionTransformTarget(FactionFog->Name, true);
    FactionFog->Exploration = RenderTarget;
    RenderTarget->bNeedsTwoCopies = true;

    UTexture* VisionTexture = nullptr;
    
    if (bUseUpscaledVision)
    {
        VisionTexture = FactionFog->UpScaledVision;
        if (VisionTexture == nullptr)
        {
            UE_LOG(LogGamekit, Warning, TEXT("Could not use Upscaled texture for exploration"));
        }
    }
 
    if (VisionTexture == nullptr)
    {
        VisionTexture = FactionFog->Vision;
    }

    /*
    auto Material = GetFactionMaterialInstance(FactionFog->Name);
    Material->SetTextureParameterValue("VisionTexture", VisionTexture);
    Material->SetTextureParameterValue("ExplorationTexture", FactionFog->Exploration);
    Material->SetScalarParameterValue("IsDiscrete", FactionFog->bDiscrete);
    */

    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), RenderTarget, Canvas, Size, Context);

    /*
    Canvas->K2_DrawMaterial(
        Material, 
        FVector2D::ZeroVector, 
        Size, 
        FVector2D::ZeroVector
    );
    */

    //*
    Canvas->K2_DrawTexture(VisionTexture,
                           FVector2D(0, 0),
                           Size,
                           FVector2D(0, 0),
                           FVector2D(1, 1),
                           FLinearColor(0, 1, 0, 0),
                           // EBlendMode::BLEND_AlphaComposite,
                           EBlendMode::BLEND_Additive,
                           0.0,
                           FVector2D(0, 0));
    //*/

    UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), Context);
}


/*
class UMaterialInstanceDynamic *UGKExplorationTransform::GetFactionMaterialInstance(FName Name)
{
    class UMaterialInstanceDynamic **Result   = MaterialInstances.Find(Name);
    class UMaterialInstanceDynamic * Instance = nullptr;

    if (Result != nullptr)
    {
        Instance = Result[0];
    }
    else
    {
        Instance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(
                GetWorld(), ExplorationMaterial, NAME_None, EMIDCreationFlags::None);

        MaterialInstances.Add(Name, Instance);
    }
    return Instance;
}
*/