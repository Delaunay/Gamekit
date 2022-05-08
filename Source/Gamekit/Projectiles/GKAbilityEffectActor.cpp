// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Projectiles/GKAbilityEffectActor.h"


// Gamekit
#include "Gamekit/Abilities/GKAbilities.h"


// Sets default values
AGKAbilityEffectActor::AGKAbilityEffectActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

void AGKAbilityEffectActor::ApplyToTargets()
{
    // Clients do not apply to target
    if (GetNetMode() == ENetMode::NM_Client){
        return;
    }

    ensure(GameplayEffects.TargetGameplayEffectSpecs.Num() > 0);
    ensure(GameplayEffects.TargetData.Data.Num() > 0);

    // Iterate list of gameplay effects
    for (const FGameplayEffectSpecHandle &SpecHandle: GameplayEffects.TargetGameplayEffectSpecs)
    {
        if (SpecHandle.IsValid())
        {
            // If effect is valid, iterate list of targets and apply to all
            for (TSharedPtr<FGameplayAbilityTargetData> Data: GameplayEffects.TargetData.Data)
            {
                Data->ApplyGameplayEffectSpec(*SpecHandle.Data.Get());
            }
        } else {
            GKGA_WARNING(TEXT("Gameplay Effect Spec Handle has become invalid"));
        }
    }
}

void AGKAbilityEffectActor::AddTargetToEffectContainerSpec(AActor *TargetActor)
{
    TArray<AActor *> TargetActors;
    TargetActors.Add(TargetActor);
    GameplayEffects.AddTargets(TArray<FHitResult>(), TargetActors);
}

void AGKAbilityEffectActor::AddHitResultToEffectContainerSpec(const FHitResult &HitResult)
{
    TArray<FHitResult> HitResults;
    HitResults.Add(HitResult);
    GameplayEffects.AddTargets(HitResults, TArray<AActor *>());
}
