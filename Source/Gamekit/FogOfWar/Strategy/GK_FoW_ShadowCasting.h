// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Container/Matrix.h"
#include "Gamekit/FogOfWar/Strategy/GK_FoW_Strategy.h"
#include "Gamekit/Grid/GKGrid.h"

// Generated
#include "GK_FoW_ShadowCasting.generated.h"

// represents the slope Y/X as a rational number
struct FGKSlope
{
    FGKSlope(int y, int x)
    {
        Y = y;
        X = x;
    }

    int Y, X;
};

enum class EGK_VisibilityLayer : uint8
{
    Block,

    LowGround,
    MediumGround,
    HighGround,

    MaxLayer,
};

enum class EGK_TileVisbility : uint8
{
    None    = 0x00, // Nothing
    Height1 = uint8(1 << 0),
    Height2 = uint8(1 << 1),
    Height3 = uint8(1 << 2),
    Height4 = uint8(1 << 3),
    Height5 = uint8(1 << 4),
    Height6 = uint8(1 << 5),
    Wall    = uint8(1 << 6), // Blocks Light
    Visible = uint8(-1), // Visible
};

enum class EGK_VisbilityLayers : uint8
{
    Terrain, // Permanent Layer
    Blocking,
    Size,
};

struct FGKPoints
{
    TArray<FIntVector> Points;
};
 
/**
 *
 * /rst
 *
 * Notes
 * -----
 *
 * Maybe we can use this version to run on the dedicated server & client.
 * In addition the client could generate rendering data to display a more
 * appealing fog of war.
 *
 * References
 * ----------
 *
 * .. [1] `Roguelike Vision Algorithms <http://www.adammil.net/blog/v125_Roguelike_Vision_Algorithms.html>`_
 * .. [2] `Symmetric Shadowcasting <https://www.albertford.com/shadowcasting/>`_
 *
 * /endrst
 *
 */
UCLASS(BlueprintType)
class UGKShadowCasting: public UGKFogOfWarStrategy
{
    GENERATED_BODY()

    public:

    //! return true when `IsVisible` is implemented
    bool SupportVisbilityQuery() const override {
        return true;
    }

    //! return true if the Seer team sees the location
    bool IsVisible(FGenericTeamId SeerTeam, FVector Loc) const override; 

    void Initialize() override;

    void DrawFactionFog(class AGKTeamFog *FactionFog) override;

    void Stop() override;

    //! Draw the line of sight using the right method
    void DrawLineOfSight(class AGKTeamFog *FactionFog, class UGKFogOfWarComponent *c) override;

    void UpdateBlocking(class UGKFogOfWarComponent *c);

    void UpdateTextures(class AGKTeamFog* TeamFog);

    void ExtractLandscapeHeight();

    class UTexture *GetFactionTexture(FName name, bool CreateRenderTarget = true) override;

    class UTexture2D *GetFactionTexture2D(FName name, bool CreateRenderTarget = true);

    class UTexture2D *GetPreviousFrameFactionTexture2D(FName name, bool bCreateRenderTarget = true);

    class UTexture2D *CreateTexture2D();

    class UTexture *GetPreviousFrameFactionTexture(FName name, bool CreateRenderTarget = true);

    void UpdatePreviousFrameTexturesTex(class AGKTeamFog* TeamFog);

    void UpdatePreviousFrameTextures(class AGKTeamFog* TeamFog); 

    private:
    void Compute(FIntVector origin, int rangeLimit, uint8 TeamId);

    void Compute(uint8      octant,
                 FIntVector origin,
                 int        rangeLimit,
                 int        x,
                 FGKSlope   top,
                 FGKSlope   bottom,
                 uint8      TeamId);

    //! BlocksLight takes X and Y coordinates of a tile and determines whether the
    //! given tile blocks the passage of light.
    //! The function must be able to accept coordinates that are out of bounds.
    bool BlocksLight(int X, int Y);

    //! GetDistance returns the distance from the point to the origin.
    int GetDistance(FIntVector Origin, FIntVector Diff);

    //! SetVisible marks a tile as visible, given its X and Y coordinates.
    //! The function ignorse coordinates that are out of bounds
    void SetVisible(int X, int Y, uint8 TeamId);

    FIntVector       TextureSize;
    FGKGrid          Grid;
    TMatrix3D<uint8> Buffer;

    UPROPERTY(Transient)
    TMap<FName, class UTexture2D*> FogFactions;

    UPROPERTY(Transient)
    TMap<FName, class UTexture2D*> PreviousFogFactions;

    class ALandscape*              Landscape;

    FUpdateTextureRegion2D         UpdateRegion;

    //
    class AGKTeamFog *             CurrentFaction;
    class UGKFogOfWarComponent *CurrentComponent;

    bool bPreviousIsPrevious;
};
