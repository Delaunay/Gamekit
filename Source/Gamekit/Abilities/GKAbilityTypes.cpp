// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Abilities/GKAbilityTypes.h"

// Gamekit
#include "Gamekit/Abilities/GKAbilitySystemComponent.h"

// Unreal Engine
#include "AbilitySystemGlobals.h"

bool FGKGameplayEffectContainerSpec::HasValidEffects() const { return TargetGameplayEffectSpecs.Num() > 0; }

bool FGKGameplayEffectContainerSpec::HasValidTargets() const { return TargetData.Num() > 0; }

void FGKGameplayEffectContainerSpec::AddTargets(const TArray<FHitResult> &HitResults,
                                                const TArray<AActor *> &  TargetActors)
{
    for (const FHitResult &HitResult: HitResults)
    {
        FGameplayAbilityTargetData_SingleTargetHit *NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
        TargetData.Add(NewData);
    }

    if (TargetActors.Num() > 0)
    {
        FGameplayAbilityTargetData_ActorArray *NewData = new FGameplayAbilityTargetData_ActorArray();
        NewData->TargetActorArray.Append(TargetActors);
        TargetData.Add(NewData);
    }
}
