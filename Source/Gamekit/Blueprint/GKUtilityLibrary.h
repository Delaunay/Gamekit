// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GKUtilityLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GAMEKIT_API UGKUtilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	//! This cannot be exposed to blueprint because fo the const
	//! It is fine though we should create accessor lile the GetMapSize
	static class AWorldSettings const* GetWorldSetting(const UObject *World);

	//! Return the map size stored inside WorldSettings
	UFUNCTION(BlueprintPure, Category = "Level|Size", meta = (WorldContext = "World"))
	static FVector2D GetWorldMapSize(const UObject *World);

	// Returns the project version set in the 'Project Settings' > 'Description' section
    // of the editor
    UFUNCTION(BlueprintPure, Category = "Project")
    static FString GetProjectVersion();

	//! Return the map size stored inside the Fog of War Volume if any
	UFUNCTION(BlueprintPure, Category = "Level|Size", meta = (WorldContext = "World"))
	static FVector2D GetFogOfWarMapSize(const UObject *World);
};
