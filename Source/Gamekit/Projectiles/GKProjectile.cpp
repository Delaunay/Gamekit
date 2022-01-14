// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.


#include "Gamekit/Projectiles/GKProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AGKProjectile::AGKProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	// We decide when we are ready to initialze movements
	ProjectileMovementComponent->bWantsInitializeComponent = false;

	bReplicates = true;

	// Sane defaults so people experimenting will get something that works
	Range = 1000;
	Behavior = EGK_ProjectileBehavior::Directional;
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

		if (DistanceTravelled >= Range) {
			Destroy();
		}
	}
}

void AGKProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGKProjectile, Speed);
	DOREPLIFETIME(AGKProjectile, Direction);
	DOREPLIFETIME(AGKProjectile, Target);
	DOREPLIFETIME(AGKProjectile, Behavior);
	DOREPLIFETIME(AGKProjectile, Range);
}

void AGKProjectile::InitProjectileMovement() {
	auto Movement = ProjectileMovementComponent;
	Movement->InitialSpeed = Speed;
	Movement->ProjectileGravityScale = 0.0f;
	Movement->bRotationFollowsVelocity = true;
	
	// Movement->bIsSliding = false;

	if (Behavior == EGK_ProjectileBehavior::Directional) {
		Movement->bIsHomingProjectile = false;
	}

	if (Behavior == EGK_ProjectileBehavior::UnitTarget && Target) {
		Movement->bIsHomingProjectile = true;
		Movement->HomingTargetComponent = Target->GetRootComponent();
		// Movement->HomingAccelerationMagnitude;
	}
}
