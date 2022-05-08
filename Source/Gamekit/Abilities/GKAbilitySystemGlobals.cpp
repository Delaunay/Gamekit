// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.
#include "Gamekit/Abilities/GKAbilitySystemGlobals.h"

UGKAbilitySystemGlobals::UGKAbilitySystemGlobals(const FObjectInitializer &ObjectInitializer): Super(ObjectInitializer)
{
}

void UGKAbilitySystemGlobals::InitGlobalTags()
{
    UAbilitySystemGlobals::InitGlobalTags();

    if (ActivateFailNotYetLearnedName != NAME_None)
    {
        ActivateFailNotYetLearnedTag = FGameplayTag::RequestGameplayTag(ActivateFailNotYetLearnedName);
    }

    if (DeathName != NAME_None)
    {
        DeathTag = FGameplayTag::RequestGameplayTag(DeathName);
    }

    if (DeathDispelName != NAME_None)
    {
        DeathDispelTag = FGameplayTag::RequestGameplayTag(DeathDispelName);
    }

    if (AnimationCastPointName != NAME_None)
    {
        AnimationCastPointTag = FGameplayTag::RequestGameplayTag(AnimationCastPointName);
    }
}
