// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Gamekit.h"

// Unreal Engine
#include "GameplayEffectExecutionCalculation.h"

// Generated
#include "GKStopMovement.generated.h"

/**
 * Stops the movement on application
 */
UCLASS(BlueprintType, Blueprintable)
class GAMEKIT_API UGKStopMovement: public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

    public:
    UGKStopMovement();

    virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters &ExecutionParams,
                                        OUT FGameplayEffectCustomExecutionOutput &OutExecutionOutput) const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UGameplayAbility> AbilityToStop;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTagContainer AbilityTagsToStop;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bUseTags;
};