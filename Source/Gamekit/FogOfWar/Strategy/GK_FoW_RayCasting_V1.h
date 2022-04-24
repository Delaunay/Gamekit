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

    //! return true when `IsVisible` is implemented
    bool SupportVisbilityQuery() const override { return true; }

    //! return true if the Seer team sees the location
    //! This might be slow; it has the query the render target directly
    //! UE usually expects that kind of thing to only happen when the editor is on
    bool IsVisible(FGenericTeamId SeerTeam, FVector Loc) const override;

    void DrawFactionFog(class AGKFogOfWarTeam *FactionFog);

    //! Draw the line of sight using the right method
    virtual void DrawLineOfSight(class AGKFogOfWarTeam *FactionFog, class UGKFogOfWarComponent *c);

    //! Retrieve the Texture used to draw the fog of war for a given faction
    virtual class UTexture *GetFactionTexture(FName name, bool bCreateRenderTarget = true);

    class UCanvasRenderTarget2D *GetFactionRenderTarget(FName name, bool bCreateRenderTarget = true);

    class UCanvasRenderTarget2D *GetFactionRenderTarget(FName Name) const;

    protected:
    //! Draw the line of sight using LineTrace
    virtual void DrawObstructedLineOfSight(class AGKFogOfWarTeam *FactionFog, UGKFogOfWarComponent *c);

    //! Draw the ligne of sight using a material (no collision)
    virtual void DrawUnobstructedLineOfSight(class AGKFogOfWarTeam *FactionFog, UGKFogOfWarComponent *c);

    void DrawUnobstructedLineOfSight_Draw(class AGKFogOfWarTeam *FactionFog, UGKFogOfWarComponent *c);

    void DrawLines(class UGKFogOfWarComponent *c);

    class UCanvasRenderTarget2D *CreateRenderTarget();

    TArray<FGKLinePoints> Lines;

    UPROPERTY(Transient)
    TMap<FName, class UCanvasRenderTarget2D *> FogFactions;

    bool DrawingFog;
};
