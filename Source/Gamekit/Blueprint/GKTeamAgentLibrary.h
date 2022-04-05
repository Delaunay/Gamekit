// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once


// Gamekit
#include "Gamekit/Gamekit.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GenericTeamAgentInterface.h"

// Generated
#include "GKTeamAgentLibrary.generated.h"


/**
 * 
 */
UCLASS()
class GAMEKIT_API UGKTeamAgentLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	static struct FGKTeamInfo const* GetTeamInfo(class UWorld* World, FGenericTeamId TeamId);

	UFUNCTION(BlueprintPure, Category = "Team", meta = (WorldContext = "World"))
    static FName GetTeamName(class UObject* World, FGenericTeamId TeamId);

	UFUNCTION(BlueprintPure, Category = "Team", meta = (WorldContext = "World"))
    static FText GetTeamDisplayName(class UObject* World, FGenericTeamId TeamId);
};
