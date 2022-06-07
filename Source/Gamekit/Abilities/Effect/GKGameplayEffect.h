//// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GKGameplayEffect.generated.h"

/**
 * 
 */
UCLASS()
class GAMEKIT_API UGKGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
	

public:
	/** Cancel all abilities that have those tags */
	// This is a wish list, I cant implement it like that because I would need to modify how 
	// `FActiveGameplayEffectsContainer::AddActiveGameplayEffectGrantedTagsAndModifiers` applies the tags
	
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags, meta = (Categories = "CancelAbilitiesWithTags"))
	// FInheritedTagContainer CancelAbilitiesWithTags;
};
