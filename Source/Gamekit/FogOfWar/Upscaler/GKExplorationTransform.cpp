// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.


#include "Gamekit/FogOfWar/Upscaler/GKExplorationTransform.h"

#include "FogOfWar/GKFogOfWarVolume.h"

#include "Engine/Canvas.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"


void UGKExplorationTransform::Transform(FGKFactionFog *FactionFog)
{
    UCanvas *                  Canvas;
    FVector2D                  Size;
    FDrawToRenderTargetContext Context;

    auto RenderTarget = GetFactionTransformTarget(FactionFog->Name, true);
    FactionFog->Exploration = RenderTarget;

    auto VisionTexture = FactionFog->UpScaledVision;
    if (VisionTexture == nullptr)
    {
        VisionTexture = FactionFog->Vision;
    }

    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), RenderTarget, Canvas, Size, Context);

    Canvas->K2_DrawTexture(VisionTexture,
                           FVector2D(0, 0),
                           Size,
                           FVector2D(0, 0),
                           FVector2D(1, 1),
                           FLinearColor(0, 1, 0, 0),
                           // EBlendMode::BLEND_Masked,
                           // EBlendMode::BLEND_MAX,
                           EBlendMode::BLEND_Additive,
                           0.0,
                           FVector2D(0, 0));

    UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), Context);
}