// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.
#pragma once

#include "Gamekit.h"

#include "Abilities/GKGameplayAbility.h"
#include "Animation/GKAnimationSet.h"

#include "Engine/DataTable.h"

#include "GKUnitStatic.generated.h"

/**
 * Character/class specific data
 */
USTRUCT(BlueprintType)
struct GAMEKIT_API FGKUnitStatic: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FGKUnitStatic():
		Health(100.0f), HealthRegen(1.0f),
		Mana(100.0f), ManaRegen(1.0f)
	{}

	//! User-visible short name 
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Name;

	//! Localized name for display
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText LocalName;

	//! Localized description for display
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText LocalDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UGKGameplayAbility>> Abilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseAttributes)
	float AttackRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseAttributes)
	float AttackAnimationPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseAttributes)
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseAttributes)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseAttributes)
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseAttributes)
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseAttributes)
	float HealthRegen;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseAttributes)
	float Mana;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = BaseAttributes)
	float ManaRegen;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	FGKAnimationSet AnimationSet;
};
