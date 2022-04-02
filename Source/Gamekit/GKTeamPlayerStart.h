// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "GKTeamPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class GAMEKIT_API AGKTeamPlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
	FName TeamName;
};
