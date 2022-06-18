// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once


// Gamekit
#include "Gamekit/Gamekit.h"
#include "Gamekit/GKGamekitSettings.h"

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
	
	static struct FGKTeamInfo const* GetTeamInfo(FGenericTeamId TeamId);

	UFUNCTION(BlueprintPure, Category = "Team")
    static FName GetTeamName( FGenericTeamId TeamId);

	UFUNCTION(BlueprintPure, Category = "Team")
    static FText GetTeamDisplayName(FGenericTeamId TeamId);

	UFUNCTION(BlueprintPure, Category = "Team")
	static void GetTeamInfo(FGenericTeamId Team, FGKTeamInfo& TeamInfo, bool& bValid);

	UFUNCTION(BlueprintPure, Category = "Team")
	static void GetTeamInfoFromName(FName Name, FGKTeamInfo& TeamInfo, bool& bValid);
};
