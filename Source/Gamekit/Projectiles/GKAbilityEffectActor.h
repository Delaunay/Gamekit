// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Abilities/GKAbilityTypes.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Generated
#include "GKAbilityEffectActor.generated.h"

//! Actor which has some game play effect attached to it
//! can add targets which should have its effect applied to
UCLASS(BlueprintType)
class GAMEKIT_API AGKAbilityEffectActor: public AActor
{
    GENERATED_BODY()

    public:
    // Sets default values for this actor's properties
    AGKAbilityEffectActor();

    public:
    // Add a target to apply the effect to
    UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
    void AddTargetToEffectContainerSpec(AActor *TargetActor);

    // Add a target to apply the effect to
    UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
    void AddHitResultToEffectContainerSpec(const FHitResult &HitResult);

    // Apply Gameplay Effect to all registered targets
    UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
    void ApplyToTargets();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Effect", Meta = (ExposeOnSpawn = true));
    FGKGameplayEffectContainerSpec GameplayEffects;
};
