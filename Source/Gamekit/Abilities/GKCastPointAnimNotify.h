// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"

#include "GKCastPointAnimNotify.generated.h"

/**
 * Specialization of a UAnimNotify that is triggered when the cast animation reach the
 * cast point and ability activation is triggered
 */
UCLASS()
class GAMEKIT_API UGKCastPointAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	void Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation) override;

	// Gameplay tag broadcasted to the owner of the ability
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Target)
	FGameplayTag CastPointEventTag;
};
