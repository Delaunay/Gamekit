// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Gamekit.h"

// Unreal Engine
#include "GameplayEffectExecutionCalculation.h"

// Generated
#include "GKDamageExecution.generated.h"

/**
 * Specify how damage are computed
 */
UCLASS()
class GAMEKIT_API UGKDamageExecution: public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

    public:
    // Constructor and overrides
    UGKDamageExecution();
    virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters &ExecutionParams,
                                        OUT FGameplayEffectCustomExecutionOutput &OutExecutionOutput) const override;
};