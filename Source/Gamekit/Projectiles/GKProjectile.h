// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Projectiles/GKAbilityEffectActor.h"

#include "GKProjectile.generated.h"

UENUM(BlueprintType)
enum class EGK_ProjectileBehavior : uint8
{
	UnitTarget  UMETA(DisplayName = "UnitTarget"),  // Target seeking Projectile
	Directional UMETA(DisplayName = "Directional"), // Projectile goes in straight line
};


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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = Projectile);
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = Projectile);
	float InitialSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = Projectile);
     float HomingAcceleration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = Projectile);
	FVector Direction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = Projectile);
	AActor* Target;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = Projectile);
	EGK_ProjectileBehavior Behavior;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = Projectile);
	float Range;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile);
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY()
	float DistanceTravelled = 0.f;

	UPROPERTY()
	FVector PreviousLoc;
};
