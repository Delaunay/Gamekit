// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.


#include "Gamekit/Abilities/GKCastPointAnimNotify.h"

#include "AbilitySystemBlueprintLibrary.h"


void UGKCastPointAnimNotify::Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	UAnimNotify::Notify(MeshComp, Animation);

	// Send Game play event
	AActor* Owner = MeshComp->GetOwner();

	// How can I populate the TargetData here
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, CastPointEventTag, FGameplayEventData());
}