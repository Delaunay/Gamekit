// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "GKEngineSubsystem.generated.h"

/**
 * Used to initialize the GAS
 */
UCLASS()
class GAMEKIT_API UGKEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
