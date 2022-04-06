// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Container/Matrix.h"

// Unreal Engine
#include "Components/ActorComponent.h"
#include "GenericTeamAgentInterface.h"
#include "CoreMinimal.h"

// Generated
#include "GK_FoW_Strategy.generated.h"

UCLASS(BlueprintType)
class GAMEKIT_API UGKFogOfWarStrategy: public UActorComponent
{
    GENERATED_BODY()

    public:
    UGKFogOfWarStrategy();

    //! return true when `IsVisible` is implemented
    virtual bool SupportVisbilityQuery() const {
        return false;
    } 

    //! return true if the Seer team sees the target actor
    virtual bool IsVisible(FGenericTeamId SeerTeam, AActor const* Target) const {
        return IsVisible(SeerTeam, Target->GetActorLocation());
    }

    virtual bool IsVisible(FGenericTeamId SeerTeam, FVector Loc) const {
        return true;
    }

    virtual void Initialize();

    virtual void Stop() {}

    //! Draw the fog of war for each factions
    virtual void DrawFactionFog(class AGKTeamFog *FactionFog);

    //! Draw the line of sight using the right method
    virtual void DrawLineOfSight(class AGKTeamFog *FactionFog, class UGKFogOfWarComponent *c) {}

    void DebugDrawComponent(class UGKFogOfWarComponent *c);

    void DebugDrawPoint(FVector Center, FLinearColor Color = FLinearColor::White, float Radius = 26.f);

    virtual TMatrix3D<uint8> const *GetFactionTextureCPU(FName name) const { return nullptr; }

    //! Retrieve the Texture used to draw the fog of war for a given faction
    virtual class UTexture *GetFactionTexture(FName name, bool CreateRenderTarget = true) { return nullptr; }

    //! Retrieve the Texture used to draw the fog of war for a given faction
    virtual class UTexture *GetPreviousFrameFactionTexture(FName name, bool CreateRenderTarget = true)
    {
        return nullptr;
    }

    void OnNewFaction(FName Name) { GetFactionTexture(Name, true); }

    // Check if actor has a FogOfWar component, is so trigger the OnSighted event
    void AddVisibleActor(class AGKTeamFog *FactionFog, class UGKFogOfWarComponent *SourceComp, class AActor *Actor);

    void AddVisibleComponent(class AGKTeamFog *      FactionFog,
                             class UGKFogOfWarComponent *SourceComp,
                             class UGKFogOfWarComponent *SightedComp);

    protected:
    class AGKFogOfWarVolume *FogOfWarVolume;
};
