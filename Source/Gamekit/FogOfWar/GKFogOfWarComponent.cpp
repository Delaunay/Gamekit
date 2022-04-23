// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/FogOfWar/GKFogOfWarComponent.h"

// Gamekit
#include "Gamekit/FogOfWar/GKFogOfWar.h"
#include "Gamekit/FogOfWar/GKFogOfWarLibrary.h"
#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"
#include "Gamekit/Blueprint/GKUtilityLibrary.h"

// Unreal Engine
#include "GenericTeamAgentInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UGKFogOfWarComponent::UGKFogOfWarComponent()
{
    // The component does not tick in itself the FogVolume will compute for everybody
    PrimaryComponentTick.bCanEverTick = false;

    // Default Settings
    Faction            = NAME_None;
    TraceCount         = 360;
    FieldOfView        = 360;
    Radius             = 600.f;
    GivesVision        = true;
    BlocksVision       = false;
    InnerRadius        = 10.f;
    UnobstructedVision = false;
    LineTickness       = 2.f;
    bWasRegistered     = false;
}

void UGKFogOfWarComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Those should not change often
    DOREPLIFETIME(UGKFogOfWarComponent, LineTickness);
    DOREPLIFETIME(UGKFogOfWarComponent, TraceCount);
    DOREPLIFETIME(UGKFogOfWarComponent, Radius);
    DOREPLIFETIME(UGKFogOfWarComponent, InnerRadius);
    DOREPLIFETIME(UGKFogOfWarComponent, UnobstructedVision);
    DOREPLIFETIME(UGKFogOfWarComponent, GivesVision);
    DOREPLIFETIME(UGKFogOfWarComponent, BlocksVision);
    DOREPLIFETIME(UGKFogOfWarComponent, FieldOfView);
}

void UGKFogOfWarComponent::BeginDestroy()
{
    auto vol = GetFogOfWarVolume();
    if (vol != nullptr)
    {
        vol->UnregisterActorComponent(this);
    }
    Super::BeginDestroy();
}

UMaterialInterface *UGKFogOfWarComponent::GetFogOfWarPostprocessMaterial()
{
    auto vol = GetFogOfWarVolume();

    if (vol == nullptr)
    {
        return nullptr;
    }

    return vol->GetFogOfWarPostprocessMaterial(Faction);
}


FGenericTeamId UGKFogOfWarComponent::GetGenericTeamId() const {
    const IGenericTeamAgentInterface *TeamAgent = Cast<const IGenericTeamAgentInterface>(GetOwner());
    return GKGETATTR(TeamAgent, GetGenericTeamId(), FGenericTeamId());
}


bool UGKFogOfWarComponent::IsVisible(AActor const* Target) const {
    return GKGETATTR(FogOfWarVolume, IsVisible(GetGenericTeamId(), Target), false);
}

bool UGKFogOfWarComponent::IsVisible(FVector Loc) const {
    return GKGETATTR(FogOfWarVolume, IsVisible(GetGenericTeamId(), Loc), false);
}

FName UGKFogOfWarComponent::DeduceFaction() const
{
    auto Settings = Cast<AGKWorldSettings>(GetWorld()->GetWorldSettings());
    
    if (!Settings)
    {
        return NAME_None;
    }

    auto TeamId   = GetGenericTeamId().GetId();
    auto TeamInfo = Settings->GetTeamInfo(TeamId);

    if (!TeamInfo || TeamInfo->Name == NAME_None)
    {
        GKFOG_WARNING(TEXT("TeamID %d is not inside the enum"), TeamId);
        return NAME_None;
    }

    return TeamInfo->Name;
}

// Called when the game starts
void UGKFogOfWarComponent::BeginPlay()
{
    Super::BeginPlay();
    bool CollisionTweaked = false;

    // Find the level volume to register itself
    auto vol = GetFogOfWarVolume();
    if (vol == nullptr)
    {
        return;
    }

    // Tweak the collision response channel
    for (UActorComponent *ActorComponent: GetOwner()->GetComponents())
    {
        UPrimitiveComponent *Primitive = Cast<UPrimitiveComponent>(ActorComponent);
        if (Primitive && Primitive->IsCollisionEnabled())
        {
            SetCollisionFoWResponse(Primitive, vol->FogOfWarCollisionChannel);
            CollisionTweaked = true;
        }
    }

    if (!CollisionTweaked)
    {
        GKFOG_LOG(TEXT("Did not find a component to set the FoW collision"));
    }

    RegisterComponent();
}

bool UGKFogOfWarComponent::RegisterComponent() {
    if (Faction == NAME_None)
    {
        Faction = DeduceFaction();
    }

    auto Volume = GetFogOfWarVolume();

    if (Volume)
    {
        if (bWasRegistered)
        {
            Volume->UnregisterActorComponent(this);
        }

        Volume->RegisterActorComponent(this);
        bWasRegistered = true;
    }

    return bWasRegistered;
}

void UGKFogOfWarComponent::SetFogOfWarMaterialParameters(UMaterialInstanceDynamic *Material)
{
    auto FoWVolume = GetFogOfWarVolume();

    if (FoWVolume == nullptr)
    {
        return;
    }

    return FoWVolume->SetFogOfWarMaterialParameters(Faction, Material);
}

void UGKFogOfWarComponent::SetCollisionFoWResponse(UPrimitiveComponent *Primitive, ECollisionChannel Channel)
{
    if (BlocksVision)
    {
        // Default
        Primitive->SetCollisionResponseToChannel(Channel, ECR_Block);
    }
    else
    {
        Primitive->SetCollisionResponseToChannel(Channel, ECR_Ignore);
    }
}

class UTexture *UGKFogOfWarComponent::GetExplorationTexture()
{
    auto FoWVolume = GetFogOfWarVolume();

    if (FoWVolume == nullptr)
    {
        return nullptr;
    }

    return FoWVolume->GetFactionExplorationTexture(Faction);
}

class UTexture *UGKFogOfWarComponent::GetVisionTexture()
{
    auto FoWVolume = GetFogOfWarVolume();

    if (FoWVolume == nullptr)
    {
        return nullptr;
    }

    return FoWVolume->GetFactionTexture(Faction);
}

AGKFogOfWarVolume *UGKFogOfWarComponent::GetFogOfWarVolume()
{
    if (FogOfWarVolume == nullptr)
    {
        TArray<AActor *> OutActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGKFogOfWarVolume::StaticClass(), OutActors);

        if (OutActors.Num() != 1)
        {
            UE_LOG(LogGamekit, Warning, TEXT("You need one FogOfWar volume per level"));
            return nullptr;
        }
        else
        {
            UE_LOG(LogGamekit, Log, TEXT("Found the level FogOfWar volume"));
        }

        FogOfWarVolume = Cast<AGKFogOfWarVolume>(OutActors[0]);
    }
    return FogOfWarVolume;
}

void UGKFogOfWarComponent::SetCameraPostprocessMaterial(UCameraComponent *CameraComponent)
{
    UGKFogOfWarLibrary::SetCameraPostprocessMaterial(FogOfWarVolume, Faction, CameraComponent);
}

FName UGKFogOfWarComponent::GetFaction()
{
    return Faction;
}