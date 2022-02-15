// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "Gamekit/FogOfWar/Strategy/GK_FoW_Strategy.h"

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "GK_FoW_RayCasting_V1.generated.h"


void BroadCastEvents(AActor *Seer, UGKFogOfWarComponent *SeerComponent, AActor *Target);


UCLASS(BlueprintType)
class GAMEKIT_API UGKRayCasting_Line: public UGKFogOfWarStrategy
{
    GENERATED_BODY()

public:
    UGKRayCasting_Line();

    virtual void Initialize();

    //! Register a new actor to the fog of war volume
    virtual void RegisterActorComponent(class UGKFogOfWarComponent *c) {}

    //! Unregister the actor to the fog of war volume
    virtual void UnregisterActorComponent(class UGKFogOfWarComponent *c) {}

    //! Draw the line of sight using the right method
    virtual void DrawLineOfSight(class UGKFogOfWarComponent *c);

    //! Retrieve the Texture used to draw the fog of war for a given faction
    virtual class UTexture *GetFactionTexture(FName name, bool bCreateRenderTarget = true);

    class UCanvasRenderTarget2D *GetFactionRenderTarget(FName name, bool bCreateRenderTarget = true);

protected:

    //! Draw the line of sight using LineTrace
    virtual void DrawObstructedLineOfSight(UGKFogOfWarComponent *c);

    //! Draw the ligne of sight using a material (no collision)
    virtual void DrawUnobstructedLineOfSight(UGKFogOfWarComponent *c);

    class UCanvasRenderTarget2D *CreateRenderTarget();

    UPROPERTY(Transient)
    TMap<FName, class UCanvasRenderTarget2D *> FogFactions;
};
