// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

// Generated
#include "GKFogOfWarLibrary.generated.h"

/**
 *
 */
UCLASS()
class GAMEKIT_API UGKFogOfWarLibrary: public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    public:
    UFUNCTION(BlueprintCallable, Category = "Collision")
    static void ConvertToObjectType(ECollisionChannel                      CollisionChannel,
                                    TArray<TEnumAsByte<EObjectTypeQuery>> &ObjectTypes);

    UFUNCTION(BlueprintCallable, Category = "FogOfWar")
    static void SetCameraPostprocessMaterial(class AGKFogOfWarVolume *Volume,
                                             FName                    Faction,
                                             class UCameraComponent * CameraComponent);
};
