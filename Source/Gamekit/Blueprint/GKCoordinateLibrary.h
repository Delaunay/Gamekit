// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

// Generated
#include "GKCoordinateLibrary.generated.h"

/**
 * Should match the shader equivalent
 */
UCLASS()
class GAMEKIT_API UGKCoordinateLibrary: public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    public:
    //! Returns coordinate (0, 1)
    UFUNCTION(BlueprintPure, Category = "Coordinate")
    static FVector2D ToTextureCoordinate(FVector WorldLocation, FVector2D MapSize);

    //! Returns coordinate (0, PixelSize)
    UFUNCTION(BlueprintPure, Category = "Coordinate")
    static FVector2D ToScreenCoordinate(FVector WorldLocation, FVector2D MapSize, FVector2D TextureSize);

    UFUNCTION(BlueprintPure, Category = "Coordinate")
    static FVector2D ToTextureCoordinate2D(FVector2D WorldLocation, FVector2D MapSize);

    UFUNCTION(BlueprintPure, Category = "Coordinate")
    static FVector2D ToScreenCoordinate2D(FVector2D WorldLocation, FVector2D MapSize, FVector2D TextureSize);
};