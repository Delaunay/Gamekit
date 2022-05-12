// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Minimap/GKMinimapVolume.h"

// Gamekit
#include "Gamekit/GKLog.h"
#include "Gamekit/Minimap/GKMinimapComponent.h"
#include "Gamekit/Blueprint/GKUtilityLibrary.h"

// Unreal Engine
#include "Components/BrushComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/Canvas.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Landscape.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialParameterCollection.h"
#include "TimerManager.h"

AGKMinimapVolume::AGKMinimapVolume()
{
    GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    GetBrushComponent()->Mobility = EComponentMobility::Static;

    MinimapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MinimapCapture"));
    MinimapCapture->SetRelativeRotation(FRotator(-90, 0, 0));
    CameraPosition = FVector(0, 0, 1000);
    MinimapCapture->SetRelativeLocation(CameraPosition);
    MinimapCapture->CaptureSource       = ESceneCaptureSource::SCS_BaseColor;
    MinimapCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
    MinimapCapture->ProjectionType      = ECameraProjectionMode::Orthographic;
    MinimapCapture->bCaptureEveryFrame  = false;

    PrimaryActorTick.bCanEverTick = true;
    bMinimapEnabled               = true;
    FramePerSeconds               = 30.f;

    FriendlyColor = FLinearColor(0, 1, 0, 1);
    NeutralColor = FLinearColor(0, 0, 1, 1);
    HostileColor = FLinearColor(1, 0, 0, 1);

    AllowClass = ALandscape::StaticClass();
}

void AGKMinimapVolume::RegisterActorComponent(UGKMinimapComponent *c)
{
    FScopeLock ScopeLock(&Mutex);
    ActorComponents.Add(c);
}

void AGKMinimapVolume::UnregisterActorComponent(UGKMinimapComponent *c)
{
    FScopeLock ScopeLock(&Mutex);
    ActorComponents.Remove(c);
}

void AGKMinimapVolume::BeginPlay()
{
    Super::BeginPlay();

    UpdateSizes();
}

void AGKMinimapVolume::Tick(float Delta) { DrawMinimap(); }

void AGKMinimapVolume::DrawMinimap()
{
    FScopeLock ScopeLock(&Mutex);

    UCanvas *                  Canvas;
    FVector2D                  Size;
    FDrawToRenderTargetContext Context;

    if (!MinimapCanvas)
    {
        return;
    }

    MinimapCapture->CaptureScene();

    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), MinimapCanvas, Canvas, Size, Context);

    UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), MinimapCanvas);

    for (auto &Component: ActorComponents)
    {
        if (!Component->Material)
            continue;

        DrawActorCompoment(Component, Canvas);
    }

    UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), Context);
}


FLinearColor AGKMinimapVolume::GetTeamColor(AActor* Actor) {
    auto CurrentAgent = Cast<IGenericTeamAgentInterface>(Actor);

    if (CurrentAgent == nullptr) {
        GK_LOG(TEXT("Actor is null"));
        return FLinearColor(1, 1, 1, 0);
    }

    auto WorldSettings = Cast<AGKWorldSettings>(GetWorld()->GetWorldSettings());

    if (WorldSettings) {
        auto TeamInfo = WorldSettings->GetTeamInfo(CurrentAgent->GetGenericTeamId().GetId());

        if (TeamInfo) {
            return TeamInfo->Color;
        }

        GK_LOG(TEXT("TeamInfo Missing"));
    }

    return FLinearColor(1, 1, 1, 0);
}


FLinearColor AGKMinimapVolume::GetTeamAptitudeColor(AActor* Actor) {
    auto PlayerController = UGKUtilityLibrary::GetFirstLocalPlayerController(GetWorld());
    auto LocalAgent = Cast<IGenericTeamAgentInterface>(PlayerController);
    auto CurrentAgent = Cast<IGenericTeamAgentInterface>(Actor);

    if (CurrentAgent == nullptr) {
        GK_LOG(TEXT("Actor is null"));
        return FLinearColor(1, 1, 1, 0);
    }

    if (LocalAgent == nullptr) {
        GK_LOG(TEXT("Local Player Controller does not implement IGenericTeamAgentInterface"));
        return FLinearColor(1, 1, 1, 0);
    }

    ETeamAttitude::Type Attitude = LocalAgent->GetTeamAttitudeTowards(*Actor);

    switch (Attitude) {
        case ETeamAttitude::Friendly: return FriendlyColor;
        case ETeamAttitude::Neutral:  return NeutralColor;
        case ETeamAttitude::Hostile:  return HostileColor;
    }

    return FLinearColor(1, 1, 1, 0);
}

FLinearColor AGKMinimapVolume::GetColor(AActor* Actor) {

    switch(ColorMode){
    case EGK_MinimapColorMode::TeamColors: return GetTeamColor(Actor);
    case EGK_MinimapColorMode::TeamAptitude: return GetTeamAptitudeColor(Actor);
    }

    return FLinearColor(1, 1, 1, 0);
}

void AGKMinimapVolume::DrawActorCompoment(UGKMinimapComponent *Compoment, UCanvas *Canvas)
{
    AActor *Actor = Compoment->GetOwner();

    auto Start = GetScreenCoordinate(Actor->GetActorLocation()) - Compoment->Size / 2.f;

    FLinearColor Color = GetColor(Actor);

    static FName ColorName = "Color";
    Compoment->MaterialInstance->SetVectorParameterValue(ColorName, FVector4(Color.R, Color.G, Color.B, Color.A));

    Canvas->K2_DrawMaterial(Cast<UMaterialInterface>(Compoment->MaterialInstance),
                            Start,                                   // Screen Position
                            FVector2D::UnitVector * Compoment->Size, // Screen Size
                            FVector2D(0.f, 0.f),                     // Coordinate
                            FVector2D::UnitVector,                   // Coordinate Size
                            - Actor->GetActorRotation().Yaw,         // Rotation
                            FVector2D(0.5f, 0.5f)                    // Pivot
    );
}

void AGKMinimapVolume::UpdateSizes()
{
    auto bb   = GetComponentsBoundingBox(true);
    auto Size = bb.GetSize();

    MapSize = FVector2D(Size.X, Size.Y);

    if (MapSize.X == 0 || MapSize.Y == 0)
    {
        GK_WARNING(TEXT("Map size too small (%.2f x %.2f)"), MapSize.X, MapSize.Y);

        // If size == 0 it will trigger assert on the RHI side
        MapSize.X = 100.f;
        MapSize.Y = 100.f;
    }

    // Make sure our Capture component is configured right
    if (MinimapCapture)
    {
        MinimapCapture->SetRelativeLocation(CameraPosition);
        MinimapCapture->OrthoWidth    = FMath::Max(MapSize.X, MapSize.Y);
        MinimapCapture->TextureTarget = MinimapCaptureTexture;

        if (ShowOnlyActors.Num() > 0)
        {
            MinimapCapture->ShowOnlyActors = ShowOnlyActors;
        }

        if (AllowClass.Get() != nullptr)
        {
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), AllowClass.Get(), MinimapCapture->ShowOnlyActors);
        }
    }
}

FVector2D AGKMinimapVolume::GetTextureSize() const { return FVector2D(MinimapCanvas->SizeX, MinimapCanvas->SizeY); }
