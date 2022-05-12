// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Unreal Engine
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

// Generated
#include "GKMinimapComponent.generated.h"

/*! \brief UGKMinimapComponent is used to define how an actor is renderer on the minimap
 */
UCLASS(Blueprintable, ClassGroup = (Minimap), meta = (BlueprintSpawnableComponent))
class GAMEKIT_API UGKMinimapComponent: public UActorComponent
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

    class AGKMinimapVolume *GetMinimapVolume();

    //! Material used to draw the actor on the minimap
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Minimap)
    class UMaterialInterface *Material;

    UPROPERTY(Transient)
    class UMaterialInstanceDynamic* MaterialInstance;

    //!
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Minimap)
    float Size;

    private:
    class AGKMinimapVolume *MinimapVolume;
};
