// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.


#include "Gamekit/Blueprint/GKCoordinateLibrary.h"


FVector2D UGKCoordinateLibrary::ToTextureCoordinate(FVector WorldLocation, FVector2D MapSize) {
	// [0.5,  - 0.5]
	auto Loation = FVector2D(WorldLocation.X, WorldLocation.Y) / MapSize;

	// [0, 1]
	return FVector2D(Loation.X + 0.5, 0.5 - Loation.Y);
}


FVector2D UGKCoordinateLibrary::ToScreenCoordinate(FVector WorldLocation, FVector2D MapSize, FVector2D TextureSize) {
    return ToTextureCoordinate(WorldLocation, MapSize) * TextureSize;
}
