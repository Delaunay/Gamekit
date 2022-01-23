// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#include "Minimap/GKMinimapVolume.h"
#include "Minimap/GKMinimapComponent.h"

#include "TimerManager.h"
#include "Components/BrushComponent.h"
#include "Engine/Canvas.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SceneCaptureComponent2D.h"


AGKMinimapVolume::AGKMinimapVolume() {
    GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    GetBrushComponent()->Mobility = EComponentMobility::Static;

    MinimapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MinimapCapture"));
    MinimapCapture->SetRelativeRotation(FRotator(-90, 0, 0));
    MinimapCapture->SetRelativeLocation(FVector(0, 0, 50));
    MinimapCapture->CaptureSource = ESceneCaptureSource::SCS_BaseColor;
    MinimapCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
    MinimapCapture->ProjectionType = ECameraProjectionMode::Orthographic;
    MinimapCapture->bCaptureEveryFrame = false;

    PrimaryActorTick.bCanEverTick = true;
    bMinimapEnabled = true;
    FramePerSeconds = 30.f;
}

void AGKMinimapVolume::RegisterActorComponent(UGKMinimapComponent* c) {
    FScopeLock ScopeLock(&Mutex);
    ActorComponents.Add(c);
}

void AGKMinimapVolume::UnregisterActorComponent(UGKMinimapComponent* c) {
    FScopeLock ScopeLock(&Mutex);
    ActorComponents.Remove(c);
}

void AGKMinimapVolume::BeginPlay() {
    Super::BeginPlay();

    UpdateSizes();


    // Avoid jittering by keeping to 2 versions
    if (MinimapCanvas)
    {
        MinimapCanvas->bNeedsTwoCopies = true;
    }
    if (MinimapCaptureTexture)
    {
        MinimapCaptureTexture->bNeedsTwoCopies = true;
    }
    // ------------

    // Start drawing the fog
    GetWorldTimerManager().SetTimer(
        MinimapComputeTimer,
        this,
        &AGKMinimapVolume::DrawMinimap,
        1.f / FramePerSeconds,
        true,
        bMinimapEnabled ? 0.f: 1.f
    );
}

void AGKMinimapVolume::DrawMinimap() {
    UCanvas* Canvas;
    FVector2D Size;
    FDrawToRenderTargetContext Context;

    if (!MinimapCanvas)
    {
        return;
    }

    MinimapCapture->CaptureScene();

    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(
        GetWorld(), 
        MinimapCanvas, 
        Canvas, 
        Size, 
        Context
    );

    UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), MinimapCanvas);

    for (auto& Component : ActorComponents) {
        if (!Component->Material)
            continue;

        DrawActorCompoment(Component, Canvas);
    }

    UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), Context);
}

void AGKMinimapVolume::DrawActorCompoment(UGKMinimapComponent *Compoment, UCanvas* Canvas) {
    AActor* Actor = Compoment->GetOwner();

    auto Start = GetScreenCoordinate(Actor->GetActorLocation()) - Compoment->Size / 2.f;

    Canvas->K2_DrawMaterial(
        Cast<UMaterialInterface>(Compoment->Material),
        Start,                                      // Screen Position
        FVector2D::UnitVector * Compoment->Size,    // Screen Size
        FVector2D(0.f, 0.f),                        // Coordinate
        FVector2D::UnitVector,                      // Coordinate Size
        0.f,                                        // Rotation
        FVector2D(0.5f, 0.5f)                       // Pivot
    );
}

void AGKMinimapVolume::UpdateSizes() {
    auto bb = GetComponentsBoundingBox(true);
    auto Size = bb.GetSize();

    MapSize = FVector2D(Size.X, Size.Y);

    if (MapSize.X == 0 || MapSize.Y == 0) {
        UE_LOG(LogGamekit, Warning, TEXT("Map size too small (%.2f x %.2f)"), MapSize.X, MapSize.Y);

        // If size == 0 it will trigger assert on the RHI side
        MapSize.X = 100.f;
        MapSize.Y = 100.f;
    }

    // Make sure our Capture component is configured right
    if (MinimapCapture)
    {
        MinimapCapture->OrthoWidth = FMath::Max(MapSize.X, MapSize.Y);
        MinimapCapture->TextureTarget = MinimapCaptureTexture;
        MinimapCapture->ShowOnlyActors = ShowOnlyActors;
    }
}

FVector2D AGKMinimapVolume::GetTextureSize() const {
    return FVector2D(MinimapCanvas->SizeX, MinimapCanvas->SizeY);
}
