// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once


// Unreal Engine
#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "Engine/DataTable.h"

// Generated
#include "GKWorldSettings.generated.h"





/** Holds all the level specific constants
 *
 */
UCLASS()
class GAMEKIT_API AGKWorldSettings: public AWorldSettings
{
    GENERATED_BODY()

    public:
    AGKWorldSettings();




    // Size of the Map in total in mm (1e3)
    // The map MUST be centered on (0, 0)
    // You can avoid using this if you are using a Fog of War Volume
    // The Volume knows its size and can be fetched using ``GetFogOfWarMapSize``
    // Which simplify your life as you do not have to set the size of everything
    // UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level|Constants")
    // FVector2D MapSize;



};
