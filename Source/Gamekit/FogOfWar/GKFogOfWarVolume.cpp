// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"

// Gamekit
#include "Gamekit/Blueprint/GKCoordinateLibrary.h"
#include "Gamekit/Blueprint/GKUtilityLibrary.h"
#include "Gamekit/FogOfWar/GKFogOfWarComponent.h"
#include "Gamekit/FogOfWar/GKFogOfWarLibrary.h"
#include "Gamekit/FogOfWar/Strategy/GK_FoW_ShadowCasting.h"
#include "Gamekit/FogOfWar/Strategy/GK_FoW_Strategy.h"
#include "Gamekit/FogOfWar/Upscaler/GKCanvasUpscaler.h"
#include "Gamekit/FogOfWar/Upscaler/GKExplorationTransform.h"
#include "Gamekit/FogOfWar/GKFogOfWarActorTeam.h"
#include "Gamekit/GKLog.h"

// Unreal Engine
#include "Components/BrushComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Canvas.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialParameterCollection.h"
#include "TimerManager.h"



void AGKFogOfWarVolume::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AGKFogOfWarVolume, TeamFogs);
}


AGKFogOfWarVolume::AGKFogOfWarVolume()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup    = TG_DuringPhysics;

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

    GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    GetBrushComponent()->Mobility = EComponentMobility::Static;

    // Defaults
    bAsyncDraw      = false;
    FramePerSeconds = 30.f;

    // Editor
    bDebug               = false;
    UseFoWDecalRendering = false;

    // Ray Cast
    TextureScale             = 1.f;
    Margin                   = 25.f;
    FogOfWarCollisionChannel = DEFAULT_FoW_COLLISION;

    // Top level
    bExploration     = true;
    bFogOfWarEnabled = true;

    // Upscaling
    FixedSize  = FIntVector(2048, 2048, 0);
    Multiplier = 2;
    bFixedSize = true;

    // Default Classes
    ExplorationClass      = UGKExplorationTransform::StaticClass();
    UpscalerClass         = UGKCanvasUpscaler::StaticClass();
    VisionDrawingStrategy = UGKShadowCasting::StaticClass();

    bReady = false;
    bReplicates = true;
    bAlwaysRelevant = true;
}

void AGKFogOfWarVolume::SetFogOfWarMaterialParameters(FName name, UMaterialInstanceDynamic *Material)
{

    UE_LOG(LogGamekit, Log, TEXT("Setting For of War Materials"));

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
    else
    {
        UE_LOG(LogGamekit, Warning, TEXT("Fog of war vision is null"));
    }

    auto PreviousFoWView = Strategy->GetPreviousFrameFactionTexture(name);
    if (PreviousFoWView != nullptr)
    {
        Material->SetTextureParameterValue("PreviousFoWView", PreviousFoWView);
    }
    else
    {
        UE_LOG(LogGamekit, Warning, TEXT("Previous Fog of war vision is null"));
    }

    auto FoWExploration = GetFactionExplorationTexture(name);
    if (FoWExploration != nullptr)
    {
        Material->SetTextureParameterValue("FoWExploration", FoWExploration);
    }
    else
    {
        UE_LOG(LogGamekit, Warning, TEXT("Fog of war exploration is null"));
    }
}

UMaterialInterface *AGKFogOfWarVolume::GetFogOfWarPostprocessMaterial(FName name)
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

    auto FoWView        = GetFactionTexture(name);
    auto FoWExploration = GetFactionExplorationTexture(name);

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
    DeltaAccumulator += DeltaTime;

    if (DeltaAccumulator < 1 / LimitFramePerSeconds)
    {
        return;
    }
    DeltaAccumulator = 0;

    if (bFogOfWarEnabled)
    {
        DrawFactionFog();

        DumpToDebugRenderTarget();
    }
}

void AGKFogOfWarVolume::InitializeStrategy()
{
    if (Strategy != nullptr)
    {
        Strategy->DestroyComponent();
    }

    Strategy = Cast<UGKFogOfWarStrategy>(AddComponentByClass(VisionDrawingStrategy, // class
                                                             false,                 // Manual Attachment
                                                             FTransform(),          // Transform
                                                             false                  // Deferred
                                                             ));

    Strategy->Initialize();
}

void AGKFogOfWarVolume::InitializeUpscaler()
{
    if (!bUpscaling)
    {
        return;
    }

    if (Upscaler != nullptr)
    {
        Upscaler->DestroyComponent();
    }

    Upscaler = Cast<UGKTransformerStrategy>(AddComponentByClass(UpscalerClass, // class
                                                                false,         // Manual Attachment
                                                                FTransform(),  // Transform
                                                                false          // Deferred
                                                                ));

    Upscaler->Initialize();
}

void AGKFogOfWarVolume::InitializeExploration()
{
    if (!bExploration)
    {
        return;
    }

    if (Exploration != nullptr)
    {
        Exploration->DestroyComponent();
    }

    Exploration = Cast<UGKTransformerStrategy>(AddComponentByClass(ExplorationClass, // class
                                                                   false,            // Manual Attachment
                                                                   FTransform(),     // Transform
                                                                   false             // Deferred
                                                                   ));

    Exploration->Initialize();
}

void AGKFogOfWarVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (Strategy != nullptr)
    {
        Strategy->Stop();
        Strategy->DestroyComponent();
    }

    if (bUpscaling)
    {
        Upscaler->Stop();
        Upscaler->DestroyComponent();
    }

    if (bExploration)
    {
        Exploration->Stop();
        Exploration->DestroyComponent();
    }
}

void AGKFogOfWarVolume::BeginPlay()
{
    DeltaAccumulator = 1000;
    Super::BeginPlay();
    PostProcessMaterials.Reset();
    UpdateVolumeSizes();

    // Reset the material instance
    DecalMaterialInstance = nullptr;
    InitDecalRendering();

    SetMatrialParams();

    InitializeStrategy();
    InitializeExploration();
    InitializeUpscaler();

    // All our strategies got created
    // create the team actor for replication
    if (GetNetMode() != ENetMode::NM_Client)
    {
        auto WorldSettings = Cast<AGKWorldSettings>(GetWorld()->GetWorldSettings());
        for (auto const &TeamInfo: WorldSettings->GetTeams())
        {
            CreateNewTeam(TeamInfo);
        }
    }

    bReady = true;

    // Start drawing the fog
    // For this method to work we need a better way to synchronize the textures
    if (bAsyncDraw)
    {
        GetWorldTimerManager().SetTimer(FogComputeTimer,
                                        this,
                                        &AGKFogOfWarVolume::DrawFactionFog,
                                        1.f / FramePerSeconds,
                                        true,
                                        bFogOfWarEnabled ? 0.f : 1.f);
    }

    if (!bFogOfWarEnabled && bAsyncDraw)
    {
        GetWorldTimerManager().PauseTimer(FogComputeTimer);
        return;
    }
}

void AGKFogOfWarVolume::RegisterActorComponent(UGKFogOfWarComponent *c)
{
    // clients cannot register components
    if (GetNetMode() == NM_Client)
    {
        return;
    }

    FScopeLock ScopeLock(&Mutex);
    ActorComponents.Add(c);
    auto Fog = GetFactionFogs(c->GetTeam());
    
    if (Fog)
    {
        Fog->Allies.Add(c);
    } else
    {
        GK_WARNING(TEXT("Coulld not add component to team"));
    }
    
    if (c->BlocksVision)
    {
        Blocking.Add(c);
    }
}

void AGKFogOfWarVolume::UnregisterActorComponent(UGKFogOfWarComponent *c)
{
    FScopeLock ScopeLock(&Mutex);
    ActorComponents.Remove(c);
    auto Fog = GetFactionFogs(c->GetTeam());
    
    if (Fog)
    {
        Fog->Allies.Remove(c);
    }
    if (c->BlocksVision)
    {
        Blocking.Remove(c);
    }
}

void AGKFogOfWarVolume::DrawFactionFog()
{
    if (!Strategy)
    {
        return;
    }

    // We are drawing to the targets we cannot change the fog components right now
    FScopeLock ScopeLock(&Mutex);

    for (auto FactionFog: TeamFogs)
    {
        Strategy->DrawFactionFog(FactionFog);

        if (bUpscaling)
        {
            Upscaler->Transform(FactionFog);
        }

        if (bExploration)
        {
            Exploration->Transform(FactionFog);
        }
    }
}

// Texture Accessors
// -----------------

UTexture *AGKFogOfWarVolume::GetFactionExplorationTexture(FName name) { 
    auto Fog = GetFactionFogs(name);
    return Fog ? Fog->Exploration: nullptr; 
}

UTexture *AGKFogOfWarVolume::GetOriginalFactionTexture(FName name) { 
     auto Fog = GetFactionFogs(name);
    return Fog ? Fog->Vision: nullptr; 
}

UTexture *AGKFogOfWarVolume::GetFactionTexture(FName name)
{
    auto Fog = GetFactionFogs(name);

    if (!Fog)
    {
        return nullptr;
    }

    if (bUpscaling)
    {
        return Fog->UpScaledVision;
    }
    return Fog->Vision;
}

// Material Parameter Collection
// -----------------------------
UMaterialParameterCollection *AGKFogOfWarVolume::GetMaterialParameterCollection() { return FogMaterialParameters; }

void AGKFogOfWarVolume::SetMatrialParams()
{
    SetMaterialParam_MapSize(MapSize);
    SetMaterialParam_TextureSize(TextureSize);
    SetMaterialParam_FoWEnabled(bFogOfWarEnabled);
    SetMaterialParam_Exploration(bExploration);
}

void AGKFogOfWarVolume::SetMaterialParam_MapSize(FVector Size)
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();
    if (MaterialParameters == nullptr)
    {
        return;
    }
    UKismetMaterialLibrary::SetVectorParameterValue(
            GetWorld(), MaterialParameters, "MapSize", FLinearColor(Size.X, Size.Y, Size.Z, 1.f));
}

void AGKFogOfWarVolume::SetMaterialParam_TextureSize(FVector2D Size)
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();
    if (MaterialParameters == nullptr)
    {
        return;
    }
    UKismetMaterialLibrary::SetVectorParameterValue(
            GetWorld(), MaterialParameters, "TextureSize", FLinearColor(Size.X, Size.Y, 1.f, 1.f));
}

void AGKFogOfWarVolume::SetMaterialParam_FoWEnabled(int Enabled)
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();
    if (MaterialParameters == nullptr)
    {
        return;
    }
    UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MaterialParameters, "FoWEnabled", Enabled);
}

void AGKFogOfWarVolume::SetMaterialParam_Exploration(int Enabled)
{
    UMaterialParameterCollection *MaterialParameters = GetMaterialParameterCollection();
    if (MaterialParameters == nullptr)
    {
        return;
    }
    UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MaterialParameters, "WithExploration", Enabled);
}

// Editor
void AGKFogOfWarVolume::PostEditChangeProperty(struct FPropertyChangedEvent &e)
{
    FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;

    if (PropertyName == GET_MEMBER_NAME_CHECKED(AGKFogOfWarVolume, VisionDrawingStrategy))
    {
        // Initializing it early does not help us
        // InitializeStrategy();
    }

    if (PropertyName == GET_MEMBER_NAME_CHECKED(AGKFogOfWarVolume, UpscalerClass))
    {
        // Initializing it early does not help us
        // InitializeStrategy();
    }

    if (PropertyName == GET_MEMBER_NAME_CHECKED(AGKFogOfWarVolume, ExplorationClass))
    {
        // Initializing it early does not help us
        // InitializeStrategy();
    }

    if (PropertyName == GET_MEMBER_NAME_CHECKED(AGKFogOfWarVolume, TextureSize))
    {
        return;
    }

    if (PropertyName == GET_MEMBER_NAME_CHECKED(AGKFogOfWarVolume, MapSize))
    {
        return;
    }

    GK_WARNING(TEXT("Property changed %s"), *PropertyName.ToString());
    UpdateVolumeSizes();
    Super::PostEditChangeProperty(e);
}


AGKFogOfWarActorTeam* AGKFogOfWarVolume::CreateNewTeam(FGKTeamInfo* TeamInfo) {
    FActorSpawnParameters SpawnInfo;
    SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnInfo.Owner                          = this;
    SpawnInfo.Instigator                     = nullptr;
    SpawnInfo.bDeferConstruction             = true;

    AGKFogOfWarActorTeam* Team = GetWorld()->SpawnActor<AGKFogOfWarActorTeam>(SpawnInfo);
    TeamFogs.Add(Team);

    Team->FogOfWar       = this;
    Team->Name           = TeamInfo->Name;
    Team->Vision         = Strategy ? Strategy->GetFactionTexture(TeamInfo->Name, true) : nullptr;
    Team->UpScaledVision = Upscaler ? Upscaler->GetFactionTexture(TeamInfo->Name, true) : nullptr;
    Team->Exploration    = Exploration ? Exploration->GetFactionTexture(TeamInfo->Name, true) : nullptr;

    UGameplayStatics::FinishSpawningActor(Team, FTransform());
    return Team;
}

AGKFogOfWarActorTeam* AGKFogOfWarVolume::GetFactionFogs(FGenericTeamId Team)
{
    if (Team.GetId() >= TeamFogs.Num())
    { 
        return nullptr; 
    }

    return TeamFogs[Team.GetId()];
}

void CopyTexture(UObject *World, class UCanvasRenderTarget2D *Dest, class UTexture *Src)
{
    UCanvas *                  Canvas;
    FVector2D                  Size;
    FDrawToRenderTargetContext Context;

    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(World, Dest, Canvas, Size, Context);

    UKismetRenderingLibrary::ClearRenderTarget2D(World, Dest);

    Canvas->K2_DrawTexture(Src, FVector2D::ZeroVector, Size, FVector2D::ZeroVector);

    UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(World, Context);
}

void AGKFogOfWarVolume::DumpToDebugRenderTarget()
{
    if (!bDebug)
    {
        return;
    }

    for (auto &DebugTexture: DebugDumpVision)
    {
        auto Texture = GetOriginalFactionTexture(DebugTexture.Key);
        CopyTexture(GetWorld(), DebugTexture.Value, Texture);
    }

    for (auto &DebugTexture: DebugDumpVisionUpscaled)
    {
        auto Texture = GetFactionTexture(DebugTexture.Key);
        CopyTexture(GetWorld(), DebugTexture.Value, Texture);
    }

    for (auto &DebugTexture: DebugDumpExploration)
    {
        auto Texture = GetFactionExplorationTexture(DebugTexture.Key);
        CopyTexture(GetWorld(), DebugTexture.Value, Texture);
    }
}