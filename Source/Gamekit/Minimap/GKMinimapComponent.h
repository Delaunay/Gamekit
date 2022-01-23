#pragma once
// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GKMinimapComponent.generated.h"

/*! \brief UGKMinimapComponent is used to define how an actor is renderer on the minimap
 */
UCLASS(Blueprintable)
class GAMEKIT_API UGKMinimapComponent : public UActorComponent
{
    GENERATED_BODY()

public:
	// Sets default values for this component's properties
    UGKMinimapComponent();

public:
	// Called when the game starts
	virtual void BeginPlay() override;

    // Unregister the component
    virtual void BeginDestroy() override;

    class AGKMinimapVolume* GetMinimapVolume();

    //! Material used to draw the actor on the minimap
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Minimap)
    class UMaterialInterface* Material;

    //!
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Minimap)
    float Size;

private:
    class AGKMinimapVolume* MinimapVolume;
};
