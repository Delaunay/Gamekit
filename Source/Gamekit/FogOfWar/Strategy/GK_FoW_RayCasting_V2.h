// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/FogOfWar/Strategy/GK_FoW_RayCasting_V1.h"

// Generated
#include "GK_FoW_RayCasting_V2.generated.h"

UCLASS(BlueprintType)
class GAMEKIT_API UGKRayCasting_Triangle: public UGKRayCasting_Line
{
    GENERATED_BODY()

    public:
    UGKRayCasting_Triangle();

    protected:
    void GenerateTriangles(UGKFogOfWarComponent *c);

    void DrawTriangles(UGKFogOfWarComponent *c);

    //! Generates Triangles of vision per actor and draw them
    //! Drawing triangles is more expensive than simple lines, you should lower
    //! the number of trace done by each actors
    //! Even with a low trace count the field of view will still render
    //! as a circle thanks to its material
    //!
    void DrawObstructedLineOfSight(struct FGKFactionFog *FactionFog, UGKFogOfWarComponent *c) override;

    UPROPERTY(Transient)
    TArray<FCanvasUVTri> Triangles;
};
