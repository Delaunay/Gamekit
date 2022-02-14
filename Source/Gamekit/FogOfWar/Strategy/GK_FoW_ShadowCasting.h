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

enum class EGK_TileVisbility : uint8
{
	None    = 0x00,	// Nothing
	Wall	= 0x7E, // Blocks Light
    Visible = 0xFF, // Visible
};


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

	//! Draw the line of sight using the right method
    void DrawLineOfSight(class UGKFogOfWarComponent *c) override;

	void UpdateBlocking(class UGKFogOfWarComponent *c);

	void UpdateTextures(class UTexture2D* Texture);

	class UTexture2D *GetFactionTexture(FName name, bool CreateRenderTarget = true) override;

private:
	void Compute(FIntVector origin, int rangeLimit);

	void Compute(uint8 octant, FIntVector origin, int rangeLimit, int x, FGKSlope top, FGKSlope bottom);

	//! BlocksLight takes X and Y coordinates of a tile and determines whether the
	//! given tile blocks the passage of light. 
	//! The function must be able to accept coordinates that are out of bounds.
	bool BlocksLight(int X, int Y);

	//! GetDistance returns the distance from the point to the origin.
    int  GetDistance(int X, int Y);

	//! SetVisible marks a tile as visible, given its X and Y coordinates.
	//! The function ignorse coordinates that are out of bounds
	void SetVisible(int X, int Y);

	FGKGrid						   Grid;
	TMatrix3D<uint8>			   Buffer;

	UPROPERTY(Transient)
	TMap<FName, class UTexture2D*> FogFactions;

	FUpdateTextureRegion2D UpdateRegion;
};
