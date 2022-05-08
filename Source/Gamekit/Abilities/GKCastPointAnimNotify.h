// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Unreal Engine
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generated
#include "GKCastPointAnimNotify.generated.h"

/**
 * Specialization of a UAnimNotify that is triggered when the cast animation reach the
 * cast point and ability activation is triggered
 */
UCLASS()
class GAMEKIT_API UGKCastPointAnimNotify: public UAnimNotify
{
    GENERATED_BODY()

    public:

    void Notify(class USkeletalMeshComponent *MeshComp, class UAnimSequenceBase *Animation, const FAnimNotifyEventReference& EventReference) override;

    // Gameplay tag broadcasted to the owner of the ability
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Target)
    FGameplayTag CastPointEventTag;

    FGameplayTag GetCastPointEventTag();
};
