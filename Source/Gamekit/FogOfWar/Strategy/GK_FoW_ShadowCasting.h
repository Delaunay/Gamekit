// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once 

#include "Gamekit/FogOfWar/Strategy/GK_FoW_Strategy.h"
#include "Gamekit/Container/Matrix.h"
#include "Gamekit/Grid/GKGrid.h"

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
    None    = 0x00,          // Nothing
    Height1 = uint8(1 << 0),
    Height2 = uint8(1 << 1),
    Height3 = uint8(1 << 2),
    Height4 = uint8(1 << 3),
    Height5 = uint8(1 << 4),
    Height6 = uint8(1 << 5),
    Wall    = uint8(1 << 6), // Blocks Light
    Visible = uint8(1 << 7), // Visible
};

#define UpscaledTextureType UTexture2D

/**
 * 
 * /rst
 * 
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
	void Initialize() override;

	void DrawFactionFog() override;

	void Stop() override;

	//! Draw the line of sight using the right method
    void DrawLineOfSight(class UGKFogOfWarComponent *c) override;

	void UpdateBlocking(class UGKFogOfWarComponent *c);

	void UpdateTextures(FName Name);

	class UTexture *GetFactionTexture(FName name, bool CreateRenderTarget = true) override;

	class UTexture2D *GetFactionTexture2D(FName name, bool CreateRenderTarget = true);

	class UTexture2D *CreateTexture2D();

	class UpscaledTextureType* GetFactionUpscaleTarget(FName name, bool bCreateRenderTarget = true);

	class UpscaledTextureType* CreateUpscaleTarget();

	bool bUpscaling;

private:
	void Compute(FIntVector origin, int rangeLimit);

	void Compute(uint8 octant, FIntVector origin, int rangeLimit, int x, FGKSlope top, FGKSlope bottom);

	//! BlocksLight takes X and Y coordinates of a tile and determines whether the
	//! given tile blocks the passage of light. 
	//! The function must be able to accept coordinates that are out of bounds.
	bool BlocksLight(int X, int Y);

	//! GetDistance returns the distance from the point to the origin.
    int GetDistance(FIntVector Origin, FIntVector Diff);

	//! SetVisible marks a tile as visible, given its X and Y coordinates.
	//! The function ignorse coordinates that are out of bounds
	void SetVisible(int X, int Y);

	FIntVector                     TextureSize;
	FGKGrid						   Grid;
	TMatrix3D<uint8>			   Buffer;

	UPROPERTY(Transient)
    TMap<FName, class UTexture2D *> FogFactions;

	UPROPERTY(Transient)
    TMap<FName, class UTexture2D*> UpscaledFogFactions;

	FUpdateTextureRegion2D UpdateRegion;

	class FUpscalingDispatcher *Uspcaler;
};
