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

    PreviewDecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> FoWPostDecalMaterial(
            TEXT("Material'/Gamekit/FogOfWar/FoWAsDecal.FoWAsDecal'"));

    if (FoWPostProcessMaterial.Succeeded())
    {
        PreviewDecalComponent->SetDecalMaterial(FoWPostDecalMaterial.Object);
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
    SetMaterialParam_MapSize(MapSize);
    SetMaterialParam_TextureSize(TextureSize);

    /*/ Resize existing Targets to match expectation
    for (auto &RenderTargets: FogFactions)
    {
        RenderTargets.Value->ResizeTarget(TextureSize.X, TextureSize.Y);
        RenderTargets.Value->bNeedsTwoCopies = true;
    }*/
}

void AGKFogOfWarVolume::GetBrushSizes(FVector2D &TextureSize_, FVector &MapSize_)
{
    auto bb   = GetComponentsBoundingBox(true);
    auto Size = bb.GetSize();

    MapSize_ = FVector(Size.X, Size.Y, Size.Z);

    if (MapSize_.X == 0 || MapSize_.Y == 0)
    {
        UE_LOG(LogGamekit, Warning, TEXT("Map size too small (%.2f x %.2f)"), MapSize_.X, MapSize_.Y);

        // If size == 0 it will trigger assert on the RHI side
        MapSize_.X = 100.f;
        MapSize_.Y = 100.f;
        MapSize_.Z = 1.f;
    }
}

void AGKFogOfWarVolume::InitDecalRendering()
{
    PreviewDecalComponent->SetVisibility(UseFoWDecalRendering);

    if (UseFoWDecalRendering && PreviewFaction.IsValid())
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

        PreviewDecalComponent->DecalSize = Size;
        if (DecalMaterialInstance == nullptr)
        {
            DecalMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(
                    GetWorld(), PreviewDecalComponent->GetDecalMaterial(), NAME_None, EMIDCreationFlags::None);
            PreviewDecalComponent->SetDecalMaterial(DecalMaterialInstance);
        }

        SetFogOfWarMaterialParameters(PreviewFaction, DecalMaterialInstance);
    }
}

void AGKFogOfWarVolume::Tick(float DeltaTime)
{
    if (bFoWEnabled)
    {
        DrawFactionFog();
    }
}

void AGKFogOfWarVolume::InitializeStrategy_Line() {
    auto DelayedStrategy = Cast<UGKRayCasting_Line>(AddComponentByClass(UGKRayCasting_Line::StaticClass(), false, FTransform(), true));
    // Initialize Properties

    // ---
    FinishAddComponent(DelayedStrategy, false, FTransform());
    Strategy = DelayedStrategy;
}

void AGKFogOfWarVolume::InitializeStrategy_Triangle() {
    auto DelayedStrategy = Cast<UGKRayCasting_Triangle>(AddComponentByClass(UGKRayCasting_Triangle::StaticClass(), false, FTransform(), true));
    // Initialize Properties

    // ---
    FinishAddComponent(DelayedStrategy, false, FTransform());
    Strategy = DelayedStrategy;
} 

void AGKFogOfWarVolume::InitializeStrategy_Less() {
    auto DelayedStrategy = Cast<UGKRayCasting_Less>(AddComponentByClass(UGKRayCasting_Less::StaticClass(), false, FTransform(), true));
    // Initialize Properties

    // ---
    FinishAddComponent(DelayedStrategy, false, FTransform());
    Strategy = DelayedStrategy;
}

void AGKFogOfWarVolume::InitializeStrategy_ShadowCasting() {
    auto DelayedStrategy = Cast<UGKShadowCasting>(AddComponentByClass(UGKShadowCasting::StaticClass(), false, FTransform(), true));
    // Initialize Properties

    // ---
    FinishAddComponent(DelayedStrategy, false, FTransform());
    Strategy = DelayedStrategy;
}


void AGKFogOfWarVolume::InitializeStrategy() {
    if (Strategy != nullptr)
    {
        Strategy->DestroyComponent();
        
    }

    switch (FogVersion)
    {
    case 1: InitializeStrategy_Line();          break;
    case 2: InitializeStrategy_Triangle();      break;
    case 3: InitializeStrategy_Less();          break;
    case 4: InitializeStrategy_ShadowCasting(); break;
    }

    Strategy->Initialize();
}

void AGKFogOfWarVolume::EndPlay(const EEndPlayReason::Type EndPlayReason) {
    if (Strategy != nullptr)
    {
        Strategy->Stop();
        Strategy->DestroyComponent();
    }
}

void AGKFogOfWarVolume::BeginPlay()
{
    Super::BeginPlay();
    ClearExploration();
    UpdateVolumeSizes();

    // Reset the material instance
    DecalMaterialInstance = nullptr;
    InitDecalRendering();

    SetMatrialParams();
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


// Material Parameter Collection
// -----------------------------
UMaterialParameterCollection *AGKFogOfWarVolume::GetMaterialParameterCollection() { return FogMaterialParameters; }

void AGKFogOfWarVolume::SetMatrialParams(){
    SetMaterialParam_MapSize(MapSize);
    SetMaterialParam_TextureSize(TextureSize);
    SetMaterialParam_FoWEnabled(bFoWEnabled);
    SetMaterialParam_Exploration(EnableExploration);
}

void AGKFogOfWarVolume::SetMaterialParam_MapSize(FVector Size)
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();
    if (MaterialParameters == nullptr){
        return;
    }
    UKismetMaterialLibrary::SetVectorParameterValue(GetWorld(), MaterialParameters, "MapSize", FLinearColor(Size.X, Size.Y, Size.Z, 1.f));
}

void AGKFogOfWarVolume::SetMaterialParam_TextureSize(FVector2D Size)
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();
    if (MaterialParameters == nullptr){
        return;
    }
    UKismetMaterialLibrary::SetVectorParameterValue(GetWorld(), MaterialParameters, "TextureSize", FLinearColor(Size.X, Size.Y, 1.f, 1.f));
}

void AGKFogOfWarVolume::SetMaterialParam_FoWEnabled(int Enabled)
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();
    if (MaterialParameters == nullptr){
        return;
    }
    UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MaterialParameters, "FoWEnabled", Enabled);
}

void AGKFogOfWarVolume::SetMaterialParam_Exploration(int Enabled)
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();
    if (MaterialParameters == nullptr){
        return;
    }
    UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MaterialParameters, "WithExploration", Enabled);
}


// Editor
void AGKFogOfWarVolume::PostEditChangeProperty(struct FPropertyChangedEvent& e) {
    FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;

    if (PropertyName == GET_MEMBER_NAME_CHECKED(AGKFogOfWarVolume, FogVersion))
    {
        // Initializing it early does not help us
        // InitializeStrategy();
    }

    if (PropertyName == GET_MEMBER_NAME_CHECKED(AGKFogOfWarVolume, TextureSize)) {
        return;
    }

    if (PropertyName == GET_MEMBER_NAME_CHECKED(AGKFogOfWarVolume, MapSize)) {
        return;
    }

    UE_LOG(LogGamekit, Warning, TEXT("Property changed %s"), *PropertyName.ToString());
    UpdateVolumeSizes();
    Super::PostEditChangeProperty(e);
}
