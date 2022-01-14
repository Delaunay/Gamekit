// BSD 3-Clause License
//
// Copyright (c) 2019, Pierre Delaunay
// All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

#include "GKHexGrid.generated.h"


UCLASS()
class GAMEKIT_API AGKHexGrid : public AActor
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	AGKHexGrid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Spacing between tiles
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Tile, meta = (AllowPrivateAccess = "true"))
	float Margin;

	// Size of the tile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Tile, meta = (AllowPrivateAccess = "true"))
	FVector2D TileSize;

	// Allow us to query the map by coordinate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Tile, meta = (AllowPrivateAccess = "true"))
	TMap<FIntVector, class UStaticMeshComponent*> GridMap;

	// Shared Tile Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Tile, meta = (AllowPrivateAccess = "true"))
	class UStaticMesh* TileMesh;

	// Tile Materials
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Tile, meta = (AllowPrivateAccess = "true"))
	TArray<class UMaterial*> TileMaterials;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = Tile)
	int AddMaterial(class UMaterial* m);

	UFUNCTION(BlueprintCallable, Category = Tile)
	class UStaticMesh* GetTileMesh() const { return TileMesh; }

	UFUNCTION(BlueprintCallable, Category = Tile)
	void SetTileMesh(class UStaticMesh* m);

	UFUNCTION(BlueprintCallable, Category = Tile)
	float GetMargin() const { return Margin; }

	UFUNCTION(BlueprintCallable, Category = Tile)
	void SetMargin(float m) { Margin = m;    }

	UFUNCTION(BlueprintCallable, Category = Tile)
	FVector2D GetTileSize() const { return TileSize + FVector2D(1.f, 1.f) * Margin;}

	UFUNCTION(BlueprintCallable, Category = MapLoading)
	void LoadRectangleMap(int radius);

	UFUNCTION(BlueprintCallable, Category = MapLoading)
	void LoadCircleMap(int radius);

	UFUNCTION(BlueprintCallable, Category = MapLoading)
	void AddTileFromWorld(FVector w, int MatIdx);

	// Insert a new tile if not existing
	UFUNCTION(BlueprintCallable, Category = MapLoading)
	void AddTile(FIntVector w, int Material);

	UFUNCTION(BlueprintCallable, Category = MapLoading)
	bool ContainsTile(FIntVector w) const;

	UFUNCTION(BlueprintCallable, Category = MapLoading)
	class UStaticMeshComponent* GetTile(FIntVector w);
};
