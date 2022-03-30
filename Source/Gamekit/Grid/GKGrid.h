#pragma once

// Gamekit
#include "Gamekit/Grid/GKHexGridUtilities.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "Math/Vector.h"
#include "Math/IntVector.h"

// Generated
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

    FGKGrid() {
        TileSize = FVector(100, 100, 50);
        GridKind = EGK_GridKind::Square;
    }

	FIntVector WorldToGrid(FVector World) const;

    FVector    SnapToGrid(FVector World) const;

    FVector    GridToWorld(FIntVector Grid) const;

    FVector    GetTileSize() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tile)
	FVector TileSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tile)
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
        World = World - TileSize / 2.f;
        return FIntVector(
            int(World.X / TileSize.X),
            int(World.Y / TileSize.Y),
            int(World.Z / TileSize.Z)
        );
    }
}

inline FVector FGKGrid::SnapToGrid(FVector World) const
{
    switch (GridKind)
    {
    case EGK_GridKind::Hexagonal:
    {
        // TODO
        return World;
    }

    case EGK_GridKind::Square:
    default:
        return FVector((FMath::GridSnap(World.X, TileSize.X)),
                       (FMath::GridSnap(World.Y, TileSize.Y)),
                       (FMath::GridSnap(World.Z, TileSize.Z)));
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
