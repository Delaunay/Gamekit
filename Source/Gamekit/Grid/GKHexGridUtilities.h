// BSD 3-Clause License
//
// Copyright (c) 2019, Pierre Delaunay
// All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "GKHexGridUtilities.generated.h"

/**
 *
 */
UCLASS()
class GAMEKIT_API UGKHexGridUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//! Round axial hex coordinate to nearest hex
	UFUNCTION(BlueprintCallable, Category="Hex|Axial")
	static FIntPoint AxialRound(FVector2D p);

	//! Round cube hex coordinate to nearest hex
	UFUNCTION(BlueprintCallable, Category="Hex|Cube")
	static FIntVector CubeRound(FVector cube);

	//! Convert cube hex coordinate to axial hex coordinate
	UFUNCTION(BlueprintCallable, Category="Hex|Cube;Hex|Axial")
	static FIntPoint CubeToAxial(FIntVector cube);

	//! Convert axial hex coordinate to cube hex coordinate
	UFUNCTION(BlueprintCallable, Category="Hex|Axial;Hex|Cube")
	static FVector AxialToCube(FVector2D hex);

	//! Convert world map position to hex grid position
	UFUNCTION(BlueprintCallable, Category="Hex|Axial;Hex|Pointy")
	static FIntVector WorldToGrid(FVector2D size, FVector world);

	//! Convert hex grid position to world map position
	UFUNCTION(BlueprintCallable, Category="Hex|Axial;Hex|Pointy")
	static FVector GridToWorld(FVector2D size, FIntVector map);

	/** Snap word coordinate to compatible grid-world coordinate
	 * size is a 2D vector using X as radius and Y as height
	 */
	UFUNCTION(BlueprintCallable, Category="Hex|Pointy")
	static FVector SnapToGrid(FVector2D size, FVector world);

	//! Return Radius/Height from a bounding box size
	UFUNCTION(BlueprintCallable, Category="Hex")
	static FVector2D GetHexSize(FVector size);

	//! Return Radius/Height from a StaticMeshComponent, useful to simplify blueprints
	UFUNCTION(BlueprintCallable, Category="Hex")
	static FVector2D GetHexSizeFromMesh(class UStaticMeshComponent* mesh);

	UFUNCTION(BlueprintCallable, Category="Hex|Axial")
	static float Distance(FIntPoint a, FIntPoint b);
};
