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
	static void ConvertToObjectType(ECollisionChannel CollisionChannel, TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes);
};
