// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Generated
#include "GKGameResource.generated.h"

UCLASS(Blueprintable)
class GAMEKIT_API AGKGameResource: public AActor
{
    GENERATED_BODY()

    public:
    // Sets default values for this actor's properties
    AGKGameResource();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resource)
    class UStaticMeshComponent *ResourceMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resource)
    FName ResourceName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resource)
    FName ResourceDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resource)
    class UObject *ResourceIcon;

    // Range of the Resource amount
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resource)
    FVector2D RarityLevel;

    // Range of the Scale
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resource)
    FVector2D RandomSize;

    // Range of the Z rotation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resource)
    FVector2D RandomRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resource)
    float CurrentResourceLevel;

    protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
};
