// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#include "GKMinimapComponent.h"
#include "GKMinimapVolume.h"

#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UGKMinimapComponent::UGKMinimapComponent()
{
	// The component does not tick in itself the FogVolume will compute for everybody
	PrimaryComponentTick.bCanEverTick = false;
}

void UGKMinimapComponent::BeginDestroy() {
    auto mini = GetMinimapVolume();

    if (mini != nullptr) {
        mini->UnregisterActorComponent(this);
    }
    Super::BeginDestroy();
}

// Called when the game starts
void UGKMinimapComponent::BeginPlay()
{
	Super::BeginPlay();

    // Find the level volume to register itself
    auto mini = GetMinimapVolume();

    if (mini == nullptr) {
        return;
    }

    mini->RegisterActorComponent(this);
}

AGKMinimapVolume* UGKMinimapComponent::GetMinimapVolume() {
    if (MinimapVolume == nullptr) {
        TArray<AActor*> OutActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGKMinimapVolume::StaticClass(), OutActors);

        if (OutActors.Num() != 1) {
            UE_LOG(LogGamekit, Warning, TEXT("You need one Minimap volume per level"));
            return nullptr;
        }
        else {
            UE_LOG(LogGamekit, Log, TEXT("Found the level Minimap volume"));
        }

        MinimapVolume = Cast<AGKMinimapVolume>(OutActors[0]);
    }
    return MinimapVolume;
}
