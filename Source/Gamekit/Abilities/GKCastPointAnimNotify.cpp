// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Abilities/GKCastPointAnimNotify.h"


// Gamekit
#include "Gamekit/Abilities/GKAbilitySystemGlobals.h"

// Unreal Engine
#include "AbilitySystemBlueprintLibrary.h"
#include "NativeGameplayTags.h"


void UGKCastPointAnimNotify::Notify(class USkeletalMeshComponent *MeshComp, class UAnimSequenceBase *Animation, const FAnimNotifyEventReference& EventReference)
{
    UAnimNotify::Notify(MeshComp, Animation, EventReference);

    // Send Game play event
    AActor *Owner = MeshComp->GetOwner();

    // this prevent the editor from printing scary error message
    // when the animation is played in the editor
    // ASC is not populated in the animation editor
    if (::IsValid(Owner) && UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner)) {
        // How can I populate the TargetData here
        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, CastPointEventTag, FGameplayEventData());
    }
}

FGameplayTag UGKCastPointAnimNotify::GetCastPointEventTag() {
    return AnimationCastPoint;
}