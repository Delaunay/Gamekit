// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.
#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"

// Generated
#include "GKEngineSubsystem.generated.h"

/**
 * Used to initialize the GAS
 */
UCLASS()
class GAMEKIT_API UGKEngineSubsystem: public UEngineSubsystem
{
    GENERATED_BODY()

    public:
    virtual void Initialize(FSubsystemCollectionBase &Collection) override;
};
