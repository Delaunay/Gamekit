// BSD 3-Clause License
//
// Copyright (c) 2022, Pierre Delaunay
// All rights reserved.

#include "Gamekit/Grid/GKHexGrid.h"

// Gamekit
#include "Gamekit/Grid/GKHexGridUtilities.h"

// Unreal Engine
#include "UObject/ConstructorHelpers.h"
#include "Materials/Material.h"
#include "Engine/StaticMesh.h"

// Sets default values
AGKHexGrid::AGKHexGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UMaterial> DefaultHexMaterial(TEXT("Material'/Gamekit/Tiles/TilePlaceholderMat.TilePlaceholderMat'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> DefaultHexMaterialAlt(TEXT("Material'/Gamekit/Tiles/TilePlaceholderMat2.TilePlaceholderMat2'"));

	if (DefaultHexMaterial.Succeeded()){
		TileMaterials.Add(DefaultHexMaterial.Object);
	}

	if (DefaultHexMaterialAlt.Succeeded()){
		TileMaterials.Add(DefaultHexMaterialAlt.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh>	TileMeshRef(TEXT("StaticMesh'/Game/Tile/HexMesh.HexMesh'"));
	if (TileMeshRef.Succeeded()){
		SetTileMesh(TileMeshRef.Object);
	}
}


void AGKHexGrid::LoadCircleMap(int radius){
	for(auto i = 0; i < radius; i++){
		for(auto j = 0; j < radius; j++){
			auto points = {
				FIntPoint( i,  j),
				FIntPoint(-i,  j),
				FIntPoint( i, -j),
				FIntPoint(-i, -j)
			};

			for (auto p: points){
				int MatIdx = 0;
				auto cube = UGKHexGridUtilities::AxialToCube(FVector2D(p.X, p.Y));
				if (cube.X == 0 || cube.Y == 0 || cube.Z == 0){
					MatIdx = 1;
				}

				if (UGKHexGridUtilities::Distance(FIntPoint(0, 0), p) <= radius * TileSize.X){
					AddTile(FIntVector(p.X, p.Y, 0), MatIdx);
				}
			}
		}
	}
}

void AGKHexGrid::LoadRectangleMap(int radius){
	for(auto i = 0; i < radius; i++){
		for(auto j = 0; j < radius; j++){
			auto points = {
				FIntPoint( i,  j),
				FIntPoint(-i,  j),
				FIntPoint( i, -j),
				FIntPoint(-i, -j)
			};

			for (auto p: points){
				int MatIdx = 0;
				auto cube = UGKHexGridUtilities::AxialToCube(FVector2D(p.X, p.Y));
				if (cube.X == 0 || cube.Y == 0 || cube.Z == 0){
					MatIdx = 1;
				}

				AddTile(FIntVector(p.X, p.Y, 0), MatIdx);
			}
		}
	}
}

void AGKHexGrid::AddTileFromWorld(FVector w, int MatIdx) {
	auto m = UGKHexGridUtilities::WorldToGrid(GetTileSize(), w);
	AddTile(m, MatIdx);
}

int AGKHexGrid::AddMaterial(class UMaterial* m){
	int size = TileMaterials.Num();

	if (m != nullptr){
		TileMaterials.Add(m);
	} else {
		return -1;
	}

	return size;
}

void AGKHexGrid::AddTile(FIntVector w, int MatIdx) {
	if (GridMap.Contains(w)){
		UE_LOG(LogTemp, Warning, TEXT("TileID: (%d x %d x %d) already there"), w.X, w.Y, w.Z);
		return;
	}

	if (MatIdx < 0 || MatIdx >= TileMaterials.Num()){
		UE_LOG(LogTemp, Warning, TEXT("(MaterialID: %d) out of bound"), MatIdx);
		return;
	}

	if (TileMesh == nullptr){
		UE_LOG(LogTemp, Warning, TEXT("Tile Mesh was not set!"));
		return;
	}

	auto mesh = NewObject<UStaticMeshComponent>();
	mesh->SetupAttachment(GetRootComponent());
	mesh->SetStaticMesh(TileMesh);
	mesh->SetMaterial(0, TileMaterials[MatIdx]);
	mesh->SetRelativeLocation(UGKHexGridUtilities::GridToWorld(GetTileSize(), w));
	GridMap.Add(w, mesh);
}

bool AGKHexGrid::ContainsTile(FIntVector w) const {
	return GridMap.Contains(w);
}

UStaticMeshComponent* AGKHexGrid::GetTile(FIntVector w) {
	return GridMap.FindRef(w);
}

void AGKHexGrid::SetTileMesh(class UStaticMesh* m) {
	if (m == nullptr)
		return;

	TileMesh = m;
	TileSize = UGKHexGridUtilities::GetHexSize(TileMesh->GetBoundingBox().GetSize());
}


// Called when the game starts or when spawned
void AGKHexGrid::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AGKHexGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

