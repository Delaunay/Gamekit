// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GenericPlatform/GenericPlatform.h"
#include "Math/IntVector.h"

#include "GKMazeGeneration.generated.h"


/**
 *
 */
UCLASS()
class GAMEKIT_API UGKMazeGeneration : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//! Builds a maze with walls randomly distributed over the grid.
	//! Returns a list of 1x1 walls that needs to be instantiated.
	UFUNCTION(BlueprintPure, Category = "Procedural|Maze")
	static void RandomWall(int GridX, int GridY, float Density, TArray<FIntVector>& Walls);

	//! Builds a maze using DepthFirstSearch
	//! Returns a list of 1x1 walls that needs to be instantiated.
	UFUNCTION(BlueprintPure, Category = "Procedural|Maze")
	static void RandomizedDepthFirstSearch(int GridX, int GridY, TArray<FIntVector>& Walls);
};
