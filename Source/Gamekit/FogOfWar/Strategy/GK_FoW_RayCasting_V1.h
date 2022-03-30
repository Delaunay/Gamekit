// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/FogOfWar/Strategy/GK_FoW_Strategy.h"

// Unreal Engine
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

// Generated
#include "GK_FoW_RayCasting_V1.generated.h"

void BroadCastEvents(AActor *Seer, UGKFogOfWarComponent *SeerComponent, AActor *Target);

struct FGKLinePoints
{
    FVector Start;
    FVector End;
};

UCLASS(BlueprintType)
class GAMEKIT_API UGKRayCasting_Line: public UGKFogOfWarStrategy
{
    GENERATED_BODY()

    public:
    UGKRayCasting_Line();

    virtual void Initialize();

    void DrawFactionFog(struct FGKFactionFog *FactionFog);

    //! Draw the line of sight using the right method
    virtual void DrawLineOfSight(struct FGKFactionFog *FactionFog, class UGKFogOfWarComponent *c);

    //! Retrieve the Texture used to draw the fog of war for a given faction
    virtual class UTexture *GetFactionTexture(FName name, bool bCreateRenderTarget = true);

    class UCanvasRenderTarget2D *GetFactionRenderTarget(FName name, bool bCreateRenderTarget = true);

    protected:
    //! Draw the line of sight using LineTrace
    virtual void DrawObstructedLineOfSight(struct FGKFactionFog *FactionFog, UGKFogOfWarComponent *c);

    //! Draw the ligne of sight using a material (no collision)
    virtual void DrawUnobstructedLineOfSight(struct FGKFactionFog *FactionFog, UGKFogOfWarComponent *c);

    void DrawUnobstructedLineOfSight_Draw(FGKFactionFog *FactionFog, UGKFogOfWarComponent *c);

    void DrawLines(class UGKFogOfWarComponent *c);

    class UCanvasRenderTarget2D *CreateRenderTarget();

    TArray<FGKLinePoints> Lines;

    UPROPERTY(Transient)
    TMap<FName, class UCanvasRenderTarget2D *> FogFactions;
};
