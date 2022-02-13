#pragma once

#include "CoreMinimal.h"
#include "Math/Vector.h"
#include "Math/IntVector.h"

#include "Gamekit/Grid/GKHexGridUtilities.h"

#include "GKGrid.generated.h"


UENUM(BlueprintType)
enum class EGK_GridKind : uint8
{
    Square,
    Hexagonal
};


USTRUCT(BlueprintType) 
struct FGKGrid
{
    GENERATED_BODY()

    public:

	FIntVector WorldToGrid(FVector World) const;

    FVector    GridToWorld(FIntVector Grid) const;

    FVector    GetTileSize() const;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Tile)
	FVector TileSize;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Tile)
    EGK_GridKind GridKind;

};


inline FIntVector FGKGrid::WorldToGrid(FVector World) const { 
    switch (GridKind)
    {
    case EGK_GridKind::Hexagonal:
    {
        auto Result = UGKHexGridUtilities::WorldToGrid(FVector2D(TileSize.X, TileSize.Y), World);
        Result.Z = int(FMath::GridSnap(World.Z, TileSize.Z));
        return Result;
    }
        
    case EGK_GridKind::Square:
    default:
        return FIntVector(
            int(FMath::GridSnap(World.X, TileSize.X)),
            int(FMath::GridSnap(World.Y, TileSize.Y)),
            int(FMath::GridSnap(World.Z, TileSize.Z))
        );
    }
}
inline FVector    FGKGrid::GridToWorld(FIntVector Grid) const {
    switch (GridKind)
    {
    case EGK_GridKind::Hexagonal:
    {
        auto Result = UGKHexGridUtilities::GridToWorld(FVector2D(TileSize.X, TileSize.Y), Grid);
        Result.Z = TileSize.Z * Grid.Z;
        return Result;
    }

    case EGK_GridKind::Square:
    default:
        return FVector(TileSize.X * Grid.X, TileSize.Y * Grid.Y, TileSize.Z * Grid.Z);
    }
}

inline FVector    FGKGrid::GetTileSize() const {
    return TileSize;
}
