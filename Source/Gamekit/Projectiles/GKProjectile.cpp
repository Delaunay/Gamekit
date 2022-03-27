// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.


#include "Projectiles/GKProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AGKProjectile::AGKProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	// We decide when we are ready to initialize movements
	ProjectileMovementComponent->bWantsInitializeComponent = false;

	bReplicates = true;
}

// Called when the game starts or when spawned
void AGKProjectile::BeginPlay()
{
	PreviousLoc = GetActorLocation();
	DistanceTravelled = 0.f;

	// TODO: can this be guaranteed ?
	// Replication should be done now; initialize movement
	InitProjectileMovement();
	ProjectileMovementComponent->InitializeComponent();

	Super::BeginPlay();
}

// Called every frame
void AGKProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!ProjectileMovementComponent->bIsHomingProjectile){
		auto CurrentLoc = GetActorLocation();
		auto Distance = FVector::Dist(CurrentLoc, PreviousLoc);
		
		if (Distance >= 0.01) {
			DistanceTravelled += Distance;
			PreviousLoc = CurrentLoc;
		}

		if (DistanceTravelled >= ProjectileData.Range)
        {
			Destroy();
		}
	}
}

void AGKProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AGKProjectile::InitProjectileMovement() {
	auto Movement = ProjectileMovementComponent;
    Movement->InitialSpeed             = ProjectileData.InitialSpeed;
    Movement->MaxSpeed                 = ProjectileData.BaseSpeed;
    Movement->ProjectileGravityScale   = 0.0f;
    Movement->Velocity                 = Direction;
	Movement->bRotationFollowsVelocity = true;
	
	// Movement->bIsSliding = false;

	if (ProjectileData.Behavior == EGK_ProjectileBehavior::Directional)
    {
		Movement->bIsHomingProjectile = false;
	}

	if (ProjectileData.Behavior == EGK_ProjectileBehavior::UnitTarget && Target)
    {
		// The accelaration needs to be very high
		Movement->bIsHomingProjectile = true;
		Movement->HomingTargetComponent = Target->GetRootComponent();
        Movement->HomingAccelerationMagnitude = ProjectileData.HomingAcceleration;
	}
}
