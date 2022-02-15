// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#include "FogOfWar/GKFogOfWarVolume.h"

#include "FogOfWar/GKFogOfWarComponent.h"
#include "FogOfWar/GKFogOfWarLibrary.h"
#include "Blueprint/GKCoordinateLibrary.h"
#include "Blueprint/GKUtilityLibrary.h"

#include "Gamekit/FogOfWar/Strategy/GK_FoW_RayCasting_V1.h"
#include "Gamekit/FogOfWar/Strategy/GK_FoW_RayCasting_V2.h"
#include "Gamekit/FogOfWar/Strategy/GK_FoW_RayCasting_V3.h"

#include "Gamekit/FogOfWar/Strategy/GK_FoW_ShadowCasting.h"

#include "Engine/CollisionProfile.h"
#include "Components/BrushComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/Canvas.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Engine/TextureRenderTarget2DArray.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialParameterCollection.h"
#include "Math/UnrealMathUtility.h"
#include "TimerManager.h"

AGKFogOfWarVolume::AGKFogOfWarVolume()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_DuringPhysics;

    static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> FoWParameterCollection(
            TEXT("MaterialParameterCollection'/Gamekit/FogOfWar/FoWParameters.FoWParameters'"));

    if (FoWParameterCollection.Succeeded())
    {
        UE_LOG(LogGamekit, Log, TEXT("Found FoW Parameter collection"));
        FogMaterialParameters = FoWParameterCollection.Object;
    }
    else
    {
        UE_LOG(LogGamekit, Warning, TEXT("Could not find FoW Parameter collection"));
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> FoWUnostructedVisionMaterial(
            TEXT("Material'/Gamekit/FogOfWar/UnobstructedVisionMaterial.UnobstructedVisionMaterial'"));

    if (FoWUnostructedVisionMaterial.Succeeded())
    {
        UnobstructedVisionMaterial = FoWUnostructedVisionMaterial.Object;
    }
    else
    {
        UE_LOG(LogGamekit, Warning, TEXT("Could not find UnobstructedVision Material"));
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> FoWPostProcessMaterial(
            TEXT("Material'/Gamekit/FogOfWar/FoWPostProcess.FoWPostProcess'"));

    if (FoWPostProcessMaterial.Succeeded())
    {
        BasePostProcessMaterial = FoWPostProcessMaterial.Object;
    }
    else
    {
        UE_LOG(LogGamekit, Warning, TEXT("Could not find Default Post Process Material"));
    }

    GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    GetBrushComponent()->Mobility = EComponentMobility::Static;

    TextureScale             = 1.f;
    FramePerSeconds          = 30.f;
    EnableExploration        = true;
    FogOfWarCollisionChannel = DEFAULT_FoW_COLLISION;
    UseFoWDecalRendering     = true;
    bFoWEnabled              = true;
    FogVersion               = 1;
    bDebug                   = false;
    Margin                   = 25.f;
    Strategy                 = nullptr;

    DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> FoWPostDecalMaterial(
            TEXT("Material'/Gamekit/FogOfWar/FoWAsDecal.FoWAsDecal'"));

    if (FoWPostProcessMaterial.Succeeded())
    {
        DecalComponent->SetDecalMaterial(FoWPostDecalMaterial.Object);
    }
    else
    {
        UE_LOG(LogGamekit, Warning, TEXT("Could not find Default Decal Material Material"));
    }
}

void AGKFogOfWarVolume::SetFogOfWarMaterialParameters(FName                     name,
                                                      UMaterialInstanceDynamic *Material,
                                                      bool                      CreateRenderTarget)
{
    if (Material == nullptr)
    {
        UE_LOG(LogGamekit, Warning, TEXT("Material is null"));
        return;
    }

    auto FoWView = GetFactionTexture(name);
    if (FoWView != nullptr)
    {
        Material->SetTextureParameterValue("FoWView", FoWView);
    }

    auto FoWExploration = GetFactionExplorationRenderTarget(name, CreateRenderTarget);
    if (FoWExploration != nullptr)
    {
        Material->SetTextureParameterValue("FoWExploration", FoWExploration);
    }
}

UMaterialInterface *AGKFogOfWarVolume::GetFogOfWarPostprocessMaterial(FName name, bool CreateRenderTarget)
{
    if (UseFoWDecalRendering)
    {
        UE_LOG(LogGamekit, Warning, TEXT("Cannot generate Post process material when using Decal Rendering"));
        return nullptr;
    }

    if (BasePostProcessMaterial == nullptr)
    {
        UE_LOG(LogGamekit, Warning, TEXT("No base post process material to create FoW material"));
        return nullptr;
    }

    UMaterialInterface **LookupResult = PostProcessMaterials.Find(name);

    if (LookupResult != nullptr)
    {
        return LookupResult[0];
    }

    // Those calls will create the render target if missing
    auto FoWView        = GetFactionTexture(name);
    auto FoWExploration = GetFactionExplorationRenderTarget(name, CreateRenderTarget);

    if (FoWView == nullptr)
    {
        UE_LOG(LogGamekit, Warning, TEXT("Missing FoWView to create FoW post process material"));
        return nullptr;
    }

    UMaterialInstanceDynamic *Material = UKismetMaterialLibrary::CreateDynamicMaterialInstance(
            GetWorld(), BasePostProcessMaterial, NAME_None, EMIDCreationFlags::None);

    if (Material == nullptr)
    {
        UE_LOG(LogGamekit, Warning, TEXT("Could not create FoW post process material"));
        return nullptr;
    }

    if (FoWView != nullptr)
    {
        Material->SetTextureParameterValue("FoWView", FoWView);
    }

    if (FoWExploration != nullptr)
    {
        Material->SetTextureParameterValue("FoWExploration", FoWExploration);
    }

    PostProcessMaterials.Add(name, Material);
    return Material;
}

void AGKFogOfWarVolume::UpdateVolumeSizes()
{
    FScopeLock ScopeLock(&Mutex);

    GetBrushSizes(TextureSize, MapSize);
    SetMapSize(MapSize);
    SetTextureSize(TextureSize);

    // Resize existing Targets to match expectation
    for (auto &RenderTargets: FogFactions)
    {
        RenderTargets.Value->ResizeTarget(TextureSize.X, TextureSize.Y);
        RenderTargets.Value->bNeedsTwoCopies = true;
    }
}

void AGKFogOfWarVolume::GetBrushSizes(FVector2D &TextureSize_, FVector2D &MapSize_)
{
    auto bb   = GetComponentsBoundingBox(true);
    auto Size = bb.GetSize();

    MapSize_ = FVector2D(Size.X, Size.Y);

    if (MapSize_.X == 0 || MapSize_.Y == 0)
    {
        UE_LOG(LogGamekit, Warning, TEXT("Map size too small (%.2f x %.2f)"), MapSize_.X, MapSize_.Y);

        // If size == 0 it will trigger assert on the RHI side
        MapSize_.X = 100.f;
        MapSize_.Y = 100.f;
    }

    if (TextureScale == 0)
    {
        UE_LOG(LogGamekit, Warning, TEXT("TextureScale cannot be zero"));

        // If size == 0 it will trigger assert on the RHI side
        TextureScale = 1.f;
    }

    TextureSize_.X = MapSize_.X * TextureScale;
    TextureSize_.Y = MapSize_.Y * TextureScale;
}

void AGKFogOfWarVolume::InitDecalRendering()
{
    DecalComponent->SetVisibility(UseFoWDecalRendering);

    if (UseFoWDecalRendering && Faction.IsValid())
    {
        auto bb   = GetComponentsBoundingBox(true);
        auto Size = bb.GetSize();

        // UE_LOG(LogGamekit, Warning, TEXT("Resizing DecalComponent %s"), *Size.ToString());
        // DecalComponent->AddLocalRotation(FQuat(FVector(0, 1, 0), -90));
        // DecalComponent->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));

        // Size returns 4000x4000x4000
        // but in the editor a decal size of 100x100x100 is enough
        // The scale of the parent is 20x20x20
        // 200x200x200 would be the unscaled probably need to device it by 2 again

        DecalComponent->DecalSize = Size;
        if (DecalMaterialInstance == nullptr)
        {
            DecalMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(
                    GetWorld(), DecalComponent->GetDecalMaterial(), NAME_None, EMIDCreationFlags::None);
            DecalComponent->SetDecalMaterial(DecalMaterialInstance);
        }

        SetFogOfWarMaterialParameters(Faction, DecalMaterialInstance);
    }
}

void AGKFogOfWarVolume::Tick(float DeltaTime)
{
    if (bFoWEnabled)
    {
        DrawFactionFog();
    }
}

void AGKFogOfWarVolume::InitializeStrategy() {
    if (Strategy != nullptr)
    {
        delete Strategy;
    }

    switch (FogVersion)
    {
    case 1:
    {
        auto DelayedStrategy = Cast<UGKRayCasting_Line>(
                AddComponentByClass(UGKRayCasting_Line::StaticClass(), false, FTransform(), true));
        FinishAddComponent(DelayedStrategy, false, FTransform());
        Strategy = DelayedStrategy;
    }
    case 2:
    {
        auto DelayedStrategy = Cast<UGKRayCasting_Triangle>(
                AddComponentByClass(UGKRayCasting_Triangle::StaticClass(), false, FTransform(), true));
        FinishAddComponent(DelayedStrategy, false, FTransform());
        Strategy = DelayedStrategy;
    }
    case 3:
    {
        auto DelayedStrategy = Cast<UGKRayCasting_Less>(
            AddComponentByClass(UGKRayCasting_Less::StaticClass(), false, FTransform(), true));
        FinishAddComponent(DelayedStrategy, false, FTransform());
        Strategy = DelayedStrategy;
    }
    case 4:
    {
        auto DelayedStrategy = Cast<UGKShadowCasting>(
            AddComponentByClass(UGKShadowCasting::StaticClass(), false, FTransform(), true));
        FinishAddComponent(DelayedStrategy, false, FTransform());
        Strategy = DelayedStrategy;
    }
    }

    Strategy->Initialize();
}

void AGKFogOfWarVolume::BeginPlay()
{
    Super::BeginPlay();

    ClearExploration();

    UpdateVolumeSizes();

    // Reset the material instance
    DecalMaterialInstance = nullptr;
    InitDecalRendering();
    SetFoWEnabledParameter(bFoWEnabled);


    // Last for now
    InitializeStrategy();

    // Start drawing the fog
    // For this method to work we need a better way to synchronize the textures
    /*
    GetWorldTimerManager().SetTimer(FogComputeTimer,
                                    this,
                                    &AGKFogOfWarVolume::DrawFactionFog,
                                    1.f / FramePerSeconds,
                                    true,
                                    bFoWEnabled ? 0.f : 1.f);
    
    if (!bFoWEnabled)
    {
        GetWorldTimerManager().PauseTimer(FogComputeTimer);
        return;
    }
    //*/
}

UCanvasRenderTarget2D *AGKFogOfWarVolume::GetFactionExplorationRenderTarget(FName name, bool CreateRenderTarget)
{
    UCanvasRenderTarget2D **renderResult = Explorations.Find(name);
    UCanvasRenderTarget2D * render       = nullptr;

    if (!EnableExploration)
    {
        return nullptr;
    }

    if (renderResult != nullptr)
    {
        render = renderResult[0];
    }
    else if (CreateRenderTarget)
    {
        GetBrushSizes(TextureSize, MapSize);

        UE_LOG(LogGamekit,
               Log,
               TEXT("Creating a new render target for %s (%.2f x %.2f)"),
               *name.ToString(),
               TextureSize.X,
               TextureSize.Y);

        render = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(
                GetWorld(), UCanvasRenderTarget2D::StaticClass(), TextureSize.X, TextureSize.Y);

        // render->MipGenSettings = TMGS_NoMipmaps;
        Explorations.Add(name, render);
    }

    return render;
}

UMaterialParameterCollection *AGKFogOfWarVolume::GetMaterialParameterCollection() { return FogMaterialParameters; }

FLinearColor AGKFogOfWarVolume::GetColor()
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();

    if (MaterialParameters == nullptr)
    {
        return FLinearColor();
    }

    FCollectionVectorParameter const *value = MaterialParameters->GetVectorParameterByName("Color");
    return value->DefaultValue;
}

FLinearColor AGKFogOfWarVolume::GetTextureSize()
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();

    if (MaterialParameters == nullptr)
    {
        return FLinearColor();
    }

    FCollectionVectorParameter const *value = MaterialParameters->GetVectorParameterByName("TextureSize");
    return value->DefaultValue;
}

FLinearColor AGKFogOfWarVolume::GetMapSize()
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();

    if (MaterialParameters == nullptr)
    {
        return FLinearColor();
    }

    FCollectionVectorParameter const *value = MaterialParameters->GetVectorParameterByName("MapSize");
    return value->DefaultValue;
}

void AGKFogOfWarVolume::SetColor(FLinearColor color)
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();

    if (MaterialParameters == nullptr)
    {
        return;
    }

    UKismetMaterialLibrary::SetVectorParameterValue(GetWorld(), MaterialParameters, "Color", color);
}

void AGKFogOfWarVolume::SetTextureSize(FVector2D size)
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();

    if (MaterialParameters == nullptr)
    {
        return;
    }

    TextureSize = size;
    UKismetMaterialLibrary::SetVectorParameterValue(
            GetWorld(), MaterialParameters, "TextureSize", FLinearColor(size.X, size.Y, 1.f, 1.f));
}

void AGKFogOfWarVolume::SetFoWEnabledParameter(bool Enabled)
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();

    if (MaterialParameters == nullptr)
    {
        return;
    }

    bFoWEnabled = Enabled;
    UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MaterialParameters, "FoWEnabled", float(bFoWEnabled));
}

void AGKFogOfWarVolume::SetMapSize(FVector2D size)
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();

    if (MaterialParameters == nullptr)
    {
        return;
    }

    MapSize = size;
    UKismetMaterialLibrary::SetVectorParameterValue(
            GetWorld(), MaterialParameters, "MapSize", FLinearColor(size.X, size.Y, 1.f, 1.f));
}

void AGKFogOfWarVolume::RegisterActorComponent(class UGKFogOfWarComponent *c)
{
    FScopeLock ScopeLock(&Mutex);
    ActorComponents.Add(c);
}

void AGKFogOfWarVolume::UnregisterActorComponent(class UGKFogOfWarComponent *c)
{
    FScopeLock ScopeLock(&Mutex);
    ActorComponents.Remove(c);
}

void AGKFogOfWarVolume::DrawFactionFog()
{
    if (!Strategy)
    {
        return;
    }

    // We are drawing to the targets we cannot change the fog components right now
    FScopeLock ScopeLock(&Mutex);

    for (auto &RenderTargets: FogFactions)
    {
        UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), RenderTargets.Value);
    }

    Strategy->DrawFactionFog();

    // Update exploration texture if any
    UpdateExploration();
}

void AGKFogOfWarVolume::ClearExploration()
{
    for (auto &RenderTargets: Explorations)
    {
        UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), RenderTargets.Value);
    }
}

void AGKFogOfWarVolume::UpdateExploration()
{
    UCanvas *                  Canvas;
    FVector2D                  Size;
    FDrawToRenderTargetContext Context;

    for (auto &RenderTargets: Explorations)
    {
        auto ExpFaction    = RenderTargets.Key;
        auto Exploration   = RenderTargets.Value;
        auto CurrentVision = GetFactionTexture(ExpFaction);

        // this check might not be necessary
        if (CurrentVision == nullptr)
        {
            continue;
        }

        UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), Exploration, Canvas, Size, Context);
        Canvas->K2_DrawTexture(CurrentVision,
                               FVector2D(0, 0),
                               Size,
                               FVector2D(0, 0),
                               FVector2D(1, 1),
                               FLinearColor(0, 1, 0, 0),
                               EBlendMode::BLEND_Additive,
                               0.0,
                               FVector2D(0, 0));
        UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), Context);
    }
}


UTexture *AGKFogOfWarVolume::GetFactionTexture(FName name) { 
    if (Strategy) {
        return Strategy->GetFactionTexture(name);
    } 

    return nullptr;
}
