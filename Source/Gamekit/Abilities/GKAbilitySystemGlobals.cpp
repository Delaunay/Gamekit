// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.
#include "Gamekit/Abilities/GKAbilitySystemGlobals.h"

// Unreal Engine
#include "NativeGameplayTags.h"

#define GKTAG(Name, Tag, DevComment)\
    FNativeGameplayTag Name(                                \
        UE_PLUGIN_NAME,                                     \
        UE_MODULE_NAME,                                     \
        Tag,                                                \
        DevComment,                                         \
        ENativeGameplayTagToken::PRIVATE_USE_MACRO_INSTEAD  \
    );

GKTAGS(GKTAG);

#undef GKTAG


UGKAbilitySystemGlobals::UGKAbilitySystemGlobals(const FObjectInitializer &ObjectInitializer): Super(ObjectInitializer)
{
}

void UGKAbilitySystemGlobals::InitGlobalTags()
{
    UAbilitySystemGlobals::InitGlobalTags();

    ActivateFailIsDeadTag       = FailureDead;
    ActivateFailCooldownTag     = FailureCooldown;
    ActivateFailCostTag         = FailureCost;
    ActivateFailTagsBlockedTag  = FailureBlocked;
    ActivateFailTagsMissingTag  = FailureMissing;
    ActivateFailNetworkingTag   = FailureNetwork;



}


UGKAbilitySystemGlobals& UGKAbilitySystemGlobals::GetGlobals() {
    return (UGKAbilitySystemGlobals&) UGKAbilitySystemGlobals::Get();
}
