// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

// Gamekit
#include "Gamekit/Abilities/GKTargetType.h"
#include "Gamekit/Abilities/GKGameplayAbility.h"
#include "Gamekit/Characters/GKCharacter.h"

void UGKTargetType::GetTargets_Implementation(AGKCharacterBase *  TargetingCharacter,
                                              AActor *            TargetingActor,
                                              FGameplayEventData  EventData,
                                              TArray<FHitResult> &OutHitResults,
                                              TArray<AActor *> &  OutActors) const
{
    return;
}

void UGKTargetType_UseOwner::GetTargets_Implementation(AGKCharacterBase *  TargetingCharacter,
                                                       AActor *            TargetingActor,
                                                       FGameplayEventData  EventData,
                                                       TArray<FHitResult> &OutHitResults,
                                                       TArray<AActor *> &  OutActors) const
{
    OutActors.Add(TargetingCharacter);
}

void UGKTargetType_UseEventData::GetTargets_Implementation(AGKCharacterBase *  TargetingCharacter,
                                                           AActor *            TargetingActor,
                                                           FGameplayEventData  EventData,
                                                           TArray<FHitResult> &OutHitResults,
                                                           TArray<AActor *> &  OutActors) const
{
    const FHitResult *FoundHitResult = EventData.ContextHandle.GetHitResult();
    if (FoundHitResult)
    {
        OutHitResults.Add(*FoundHitResult);
    }
    else if (EventData.Target)
    {
        OutActors.Add(const_cast<AActor *>(EventData.Target));
    }
}