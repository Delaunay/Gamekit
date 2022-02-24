// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#include "GKFogOfWarComponent.h"
#include "GKFogOfWarVolume.h"

#include "Gamekit/FogOfWar/GKFogOfWarLibrary.h"

#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UGKFogOfWarComponent::UGKFogOfWarComponent()
{
	// The component does not tick in itself the FogVolume will compute for everybody
	PrimaryComponentTick.bCanEverTick = false;

    // Default Settings
    Faction = "Default";
    TraceCount = 360;
    FieldOfView = 360;
    Radius = 600.f;
    GivesVision = true;
    BlocksVision = false;
    InnerRadius = 10.f;
    UnobstructedVision = false;
    LineTickness = 2.f;
}

void UGKFogOfWarComponent::BeginDestroy() {
    auto vol = GetFogOfWarVolume();
    if (vol != nullptr) {
        vol->UnregisterActorComponent(this);
    }
    Super::BeginDestroy();
}

UMaterialInterface* UGKFogOfWarComponent::GetFogOfWarPostprocessMaterial() {
    auto vol = GetFogOfWarVolume();

    if (vol == nullptr) {
        return nullptr;
    }

    return vol->GetFogOfWarPostprocessMaterial(Faction);
}

// Called when the game starts
void UGKFogOfWarComponent::BeginPlay()
{
	Super::BeginPlay();
    bool CollisionTweaked = false;

    // Find the level volume to register itself
    auto vol = GetFogOfWarVolume();
    if (vol == nullptr) {
        return;
    }

    // Tweak the collision response channel
    for (UActorComponent* ActorComponent : GetOwner()->GetComponents())
    {
        UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(ActorComponent);
        if (Primitive && Primitive->IsCollisionEnabled())
        {
            SetCollisionFoWResponse(Primitive, vol->FogOfWarCollisionChannel);
            CollisionTweaked = true;
        }
    }

    if (!CollisionTweaked) {
        UE_LOG(LogGamekit, Log, TEXT("Did not find a component to set the FoW collision"));
    }

    vol->RegisterActorComponent(this);
}

void UGKFogOfWarComponent::SetFogOfWarMaterialParameters(UMaterialInstanceDynamic* Material) {
    auto FoWVolume = GetFogOfWarVolume();

    if (FoWVolume == nullptr) {
        return;
    }

    return FoWVolume->SetFogOfWarMaterialParameters(Faction, Material);
}

void UGKFogOfWarComponent::SetCollisionFoWResponse(UPrimitiveComponent* Primitive, ECollisionChannel Channel) {
    if (BlocksVision) {
        // Default
        Primitive->SetCollisionResponseToChannel(Channel, ECR_Block);
    }
    else {
        Primitive->SetCollisionResponseToChannel(Channel, ECR_Ignore);
    }
}

class UTexture* UGKFogOfWarComponent::GetVisionTexture() {
    auto FoWVolume = GetFogOfWarVolume();

    if (FoWVolume == nullptr) {
        return nullptr;
    }

    return FoWVolume->GetFactionTexture(Faction);
}

AGKFogOfWarVolume* UGKFogOfWarComponent::GetFogOfWarVolume() {
    if (FogOfWarVolume == nullptr) {
        TArray<AActor*> OutActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGKFogOfWarVolume::StaticClass(), OutActors);
        
        if (OutActors.Num() != 1) {
            UE_LOG(LogGamekit, Warning, TEXT("You need one FogOfWar volume per level"));
            return nullptr;
        }
        else {
            UE_LOG(LogGamekit, Log, TEXT("Found the level FogOfWar volume"));
        }

        FogOfWarVolume = Cast<AGKFogOfWarVolume>(OutActors[0]);
    }
    return FogOfWarVolume;
}

void UGKFogOfWarComponent::SetCameraPostprocessMaterial(UCameraComponent *CameraComponent) { 
    UGKFogOfWarLibrary::SetCameraPostprocessMaterial(FogOfWarVolume, Faction, CameraComponent);
}