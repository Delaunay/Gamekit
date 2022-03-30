// BSD 3-Clause License
//
// Copyright (c) 2022, Pierre Delaunay
// All rights reserved.

// Gamekit
#include "Gamekit/Grid/GKHexGridUtilities.h"

// Unreal Engine
#include "Engine/StaticMesh.h"

#include <cmath>

// using Vec2 = FVector2D
// using Vec2i = FIntPoint
// using Vec3 = FVector
// using FVectori = FIntVector

//        Blender	      UE4
// Radius	  1.000	  100.000
// W	      2.000	  200.000	150.000
// H   	    1.732	  173.205	 86.603


FIntPoint UGKHexGridUtilities::AxialRound(FVector2D p){
	return CubeToAxial(CubeRound(AxialToCube(p)));
}

FIntVector UGKHexGridUtilities::CubeRound(FVector cube){
	int rx = round(cube.X);
	int ry = round(cube.Y);
	int rz = round(cube.Z);

	float x_diff = abs(rx - cube.X);
	float y_diff = abs(ry - cube.Y);
	float z_diff = abs(rz - cube.Z);

	if (x_diff > y_diff && x_diff > z_diff)
		rx = -ry-rz;
	else if (y_diff > z_diff)
		ry = -rx-rz;
	else
		rz = -rx-ry;

	return FIntVector(rx, ry, rz);
}

FIntPoint UGKHexGridUtilities::CubeToAxial(FIntVector cube){
	int q = cube.X;
	int r = cube.Z;
	return FIntPoint(q, r);
}

FVector UGKHexGridUtilities::AxialToCube(FVector2D hex) {
	float x = hex.X;
	float z = hex.Y;
	float y = -x - z;
	return FVector(x, y, z);
}

FIntVector UGKHexGridUtilities::WorldToGrid(FVector2D size, FVector world){
	// Flat
//	float q = ( 2./3. * world.X                        ) / size.X;
//	float r = (-1./3. * world.X + sqrt(3.)/3. * world.Y) / size.X;
//	float z = world.Z / size.Y;

//	auto rounded = AxialRound(FVector2D(q, r));
//	return FIntVector(rounded.X, rounded.Y, round(z));

	// Pointy
	float q = (sqrt(3.)/3. * world.X - 1./3. * world.Y) / size.X;
	float r = (2./3. * world.Y) / size.X;
	float z = world.Z / size.Y;

	auto rounded = AxialRound(FVector2D(q, r));
	return FIntVector(rounded.X, rounded.Y, round(z));
}

FVector UGKHexGridUtilities::GridToWorld(FVector2D size, FIntVector map) {
	// Flat
	//	float x = size.X * (3./2. * float(map.X));
	//	float y = size.X * (sqrt(3.)/2. * float(map.X)  +  sqrt(3.) * float(map.Y));
	//	return FVector(x, y, float(map.Z) * size.Y);

	// Pointy
	float x = size.X * (sqrt(3.) * float(map.X) + sqrt(3.) / 2. * float(map.Y));
	float y = size.X * (3. / 2. * float(map.Y));
	return FVector(x, y, float(map.Z) * size.Y);
}

FVector UGKHexGridUtilities::SnapToGrid(FVector2D size, FVector world) {
	return GridToWorld(size, WorldToGrid(size, world));
}

FVector2D UGKHexGridUtilities::GetHexSize(FVector size) {
	// Works for flat and pointy top Hex
	return FVector2D(fmax(size.X, size.Y) / 2.f, size.Z);
}

FVector2D UGKHexGridUtilities::GetHexSizeFromMesh(class UStaticMeshComponent* mesh) {
	return GetHexSize(mesh->GetStaticMesh()->GetBoundingBox().GetSize());
}

float Distance(FIntVector a, FIntVector b) {
	return FMath::Max3(abs(a.X - b.X), abs(a.Y - b.Y), abs(a.Z - b.Z));
}

float UGKHexGridUtilities::Distance(FIntPoint a, FIntPoint b) {
	auto aa = AxialToCube(a);
	auto bb = AxialToCube(b);
	return ::Distance(FIntVector(aa.X, aa.Y, aa.Z), FIntVector(bb.X, bb.Y, bb.Z));
}
