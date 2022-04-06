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
    
    //! return true if the actor is visible by current client
    //! This is based off the clients own drawing of the fog
    //! so in case of cheating this method would be incorrect
    //! but that would only mean that the client is seeing the latest known position
    //! of the enemies since the enemies replicate only if visible (according to the server)
    UFUNCTION(BlueprintCallable, Category = "FogOfWar", meta = (WorldContext = "WorldCtx"))
    static bool IsVisible(UObject* WorldCtx, AActor const* Target);
};
