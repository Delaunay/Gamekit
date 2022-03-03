// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GKFogOfWarLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GAMEKIT_API UGKFogOfWarLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Collision")
	static void ConvertToObjectType(ECollisionChannel CollisionChannel, TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes);

	UFUNCTION(BlueprintCallable, Category = "FogOfWar")
	static void SetCameraPostprocessMaterial(class AGKFogOfWarVolume *Volume,
                                             FName                    Faction,
                                             class UCameraComponent * CameraComponent);
};
