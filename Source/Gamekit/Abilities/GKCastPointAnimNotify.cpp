// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Abilities/GKCastPointAnimNotify.h"

// Unreal Engine
#include "AbilitySystemBlueprintLibrary.h"

void UGKCastPointAnimNotify::Notify(class USkeletalMeshComponent *MeshComp, class UAnimSequenceBase *Animation, const FAnimNotifyEventReference& EventReference)
{
    UAnimNotify::Notify(MeshComp, Animation, EventReference);

    // Send Game play event
    AActor *Owner = MeshComp->GetOwner();

    // How can I populate the TargetData here
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, CastPointEventTag, FGameplayEventData());
}