// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Projectiles/GKAbilityEffectActor.h"
#include "Projectiles/GKProjectileStatic.h"

#include "GKProjectile.generated.h"


UCLASS(BlueprintType)
class GAMEKIT_API AGKProjectile : public AGKAbilityEffectActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGKProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void InitProjectileMovement();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile);
	FGKProjectileStatic ProjectileData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile);
	FVector Direction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = Projectile);
	AActor* Target;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile);
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY()
	float DistanceTravelled = 0.f;

	UPROPERTY()
	FVector PreviousLoc;
};
