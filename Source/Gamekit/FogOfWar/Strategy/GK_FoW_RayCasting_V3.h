// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "Gamekit/FogOfWar/Strategy/GK_FoW_RayCasting_V2.h"

#include "GK_FoW_RayCasting_V3.generated.h"


UCLASS(BlueprintType)
class GAMEKIT_API UGKRayCasting_Less: public UGKRayCasting_Triangle
{
    GENERATED_BODY()

public:
    UGKRayCasting_Less();

protected:
    //! V3 find all the obstacle and try to draw more traces neat the bounds
    //! There is a sorting problem, the angles behaves a bit strangely
    //! although they are sorted the triangles are not drawn correctly
    //! 
    //!     * Generate minimum Line traces <----------------------------+
    //!     * Get all obstacle in a radius                              |
    //!     * For each obstacle add 2 traces                            |
    //!     * Add Line traces if angle between 2 traces are too wide <--+
    //!     * Generate triangles
    //!     * Draw
    void DrawObstructedLineOfSight(struct FGKFactionFog *FactionFog, UGKFogOfWarComponent *c) override;

    void CastLinesFromAngles(struct FGKFactionFog *FactionFog, UGKFogOfWarComponent *c, TArray<float> &Angles);

    //! Make sure we do a full turn
    //! TODO: support field of view != 360
    void FillMissingAngles(UGKFogOfWarComponent *c, TArray<float> &Angles);

    void Generate3Triangles(FGKFactionFog *       FactionFog, 
                            UGKFogOfWarComponent *c, 
                            FVector4 const &      Angles);

    FGKLinePoints CastLineFromAngle(FGKFactionFog *FactionFog, UGKFogOfWarComponent *c, float Angle);
     
    void GenerateTriangle(UGKFogOfWarComponent *c, FVector Start, FVector End1, FVector End2);

    TArray<FVector2D> Coverage;


    private:
    TArray<AActor *> ActorsToIgnore;
    UClass *         ActorClassFilter;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    AActor *         ComponentOwner;
    FVector          OwnerLocation;
    FVector          OwnerForward;
    FHitResult       OutHit;
    ETraceTypeQuery  TraceType;
    TSet<AActor *>   AlreadySighted;
    bool             bHasPrevious;
    float            PreviousAngle;
};
